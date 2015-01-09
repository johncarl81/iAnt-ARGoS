#include "iAnt_controller.h"

// exponential decay
static inline float exponentialDecay(float quantity, float time, float lambda) {
	return (quantity * exp(-lambda * time));
}

//Provides bound on value by rolling over a la modulo
static inline double bound(double x, double min, double max) {
    double offset = Abs(min) + Abs(max);
    while (x < min) {
        x += offset;
    }
    while (x > max) {
        x -= offset;
    }
    return x;
}

// Returns Poisson cumulative probability at a given k and lambda
static inline float poissonCDF(float k, float lambda) {
    float sumAccumulator = 1;
    float factorialAccumulator = 1;

    for (int i = 1; i <= floor(k); i++) {
        factorialAccumulator *= i;
        sumAccumulator += pow(lambda, i) / factorialAccumulator;
    }

    return (exp(-lambda) * sumAccumulator);
}

// constructor, see iAnt_controller::Init(TConfigurationNode& node);
iAnt_controller::iAnt_controller():
	steeringActuator(NULL),
	proximitySensor(NULL),
	groundSensor(NULL),
	compassSensor(NULL),
	RNG(NULL),
	holdingFood(false),
	informed(false),
	collisionDelay(0),
	resourceDensity(0),
    nestRadiusSquared(0.0),
    foodRadiusSquared(0.0),
	searchRadiusSquared(0.0),
	distanceTolerance(0.0),
	travelGiveupProbability(0.0),
	searchGiveupProbability(0.0),
	searchStepSize(0.0),
	maxSpeed(0.0),
	informedSearchDecay(0.0),
	siteFidelityRate(0.0),
	pheromoneRate(0.0),
	pheromoneDecayRate(0.0),
	simTime(0),
	searchTime(0),
	CPFA(INACTIVE)
{}

// destructor
iAnt_controller::~iAnt_controller() {
	// not in use
}

/* iAnt_controller Class Initialization Function
 *
 * Set the iAnt_controller class variables from data in a *.argos XML file. This function is
 * inherited from the CCI_Controller class.
 *
 * @param     node     ARGoS XML configuration node
 */
void iAnt_controller::Init(TConfigurationNode& node) {
	// Initialize ARGoS sensors and actuators from these categories in the *.argos XML file.
	steeringActuator = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
	proximitySensor  = GetSensor<CCI_FootBotProximitySensor>        ("footbot_proximity"    );
	groundSensor     = GetSensor<CCI_FootBotMotorGroundSensor>      ("footbot_motor_ground" );
    compassSensor    = GetSensor<CCI_PositioningSensor>             ("positioning"          );

	GetNodeAttribute(GetNode(node, "navigation"), "searchRadius"           , searchRadiusSquared);
	GetNodeAttribute(GetNode(node, "navigation"), "distanceTolerance"      , distanceTolerance);
	GetNodeAttribute(GetNode(node, "navigation"), "searchGiveupProbability", searchGiveupProbability);
	GetNodeAttribute(GetNode(node, "navigation"), "searchStepSize"         , searchStepSize);
	GetNodeAttribute(GetNode(node, "navigation"), "maxSpeed"               , maxSpeed);
	GetNodeAttribute(GetNode(node, "CPFA"      ), "informedSearchDecay"    , informedSearchDecay);
	GetNodeAttribute(GetNode(node, "CPFA"      ), "siteFidelityRate"       , siteFidelityRate);
	GetNodeAttribute(GetNode(node, "CPFA"      ), "pheromoneRate"          , pheromoneRate);
	GetNodeAttribute(GetNode(node, "CPFA"      ), "pheromoneDecayRate"     , pheromoneDecayRate);
	GetNodeAttribute(GetNode(node, "CPFA"      ), "travelGiveupProbability", travelGiveupProbability);

	targetPheromone.SetDecay(pheromoneDecayRate);
	sharedPheromone.SetDecay(pheromoneDecayRate);

	// We get input in degrees from the XML file for the user's ease of use.
	CDegrees angleInDegrees;
    GetNodeAttribute(GetNode(node, "navigation"), "uninformedSearchCorrelation", angleInDegrees);

    // Convert the input from angleInDegrees to Radians.
    uninformedSearchCorrelation = ToRadians(angleInDegrees);

    GetNodeAttribute(GetNode(node, "navigation"), "angleTolerance", angleInDegrees);
    // Convert the input from angleInDegrees to radians.
    angleTolerance.Set(-ToRadians(angleInDegrees), ToRadians(angleInDegrees));

    // Square all of the Squared input variables
    searchRadiusSquared *= searchRadiusSquared;

	// Initialize the random number generator using the "random_seed" found in the XML file.
	RNG = CRandom::CreateRNG("argos");
}

/*
 * iAnt_controller Helper Function
 *
 * The CCI_FootBotMotorGroundSensor sensors are built on the motor PCB and they are located close
 * to the motors. There are four sensors and are useful to detect changes in color on the ground.
 *
 * The readings are in the following order (seeing the robot from TOP, battery socket is the BACK):
 *
 *   /---[front]---\    The color values read by these sensors ranges in value from 0.0 to 1.0,
 * l|w             r|w  where 0.0 = black and 1.0 = white. iAnt_loop_functions uses the predefined
 * e|h   1     0   i|h  CColor gray-scale colors to set up various objects on the field.
 * f|e             g|e
 * t|e   2     3   h|e  Arena Floor = [CColor::WHITE]  reading value is approximately 1.00
 *  |l             t|l  Food Item   = [CColor::BLACK]  reading value is approximately 0.00
 *   \---[back ]---/    Nest Zone   = [CColor::GRAY80] reading value is approximately 0.80
 *                      Pheromone   = [CColor::GRAY40] reading value is approximately 0.40
 *
 * This function is used to determine if the robot is currently inside the Nest Zone.
 *
 * @return     bool     "true" if the robot is currently within the borders of the nest,
 *                      "false" if the robot is not currently within the borders of the nest
 */
bool iAnt_controller::IsInTheNest() {
    /*
	// Obtain the current ground sensor readings for this controller object.
	const CCI_FootBotMotorGroundSensor::TReadings &groundReadings = groundSensor->GetReadings();

	// The ideal value is 0.8, but we must account for sensor read errors (+/- 0.1).
	CRange<Real> nestSensorRange(0.7, 0.9);

	// Assign the ground readings to temporary variables for clarity.
	Real backLeftWheelReading  = groundReadings[2].Value;
	Real backRightWheelReading = groundReadings[3].Value;

	// We only need to check the back side sensors, if these are in the nest so is the front.
	if(nestSensorRange.WithinMinBoundIncludedMaxBoundIncluded(backLeftWheelReading) &&
	   nestSensorRange.WithinMinBoundIncludedMaxBoundIncluded(backRightWheelReading)) {
	    return true; // The robot is in the nest zone.
	}
    */

    if((position - nestPosition).SquareLength() < GetNestRadius()) {
        return true; // The robot is in the nest.
    }

	return false; // The robot is not in the nest.
}

/*
 * iAnt_controller Helper Function
 *
 * The CCI_FootBotMotorGroundSensor sensors are built on the motor PCB and they are located close
 * to the motors. There are four sensors and are useful to detect changes in color on the ground.
 *
 * The readings are in the following order (seeing the robot from TOP, battery socket is the BACK):
 *
 *   /---[front]---\    The color values read by these sensors ranges in value from 0.0 to 1.0,
 * l|w             r|w  where 0.0 = black and 1.0 = white. iAnt_loop_functions uses the predefined
 * e|h   1     0   i|h  CColor gray-scale colors to set up various objects on the field.
 * f|e             g|e
 * t|e   2     3   h|e  Arena Floor = [CColor::WHITE]  reading value is approximately 1.00
 *  |l             t|l  Food Item   = [CColor::BLACK]  reading value is approximately 0.00
 *   \---[back ]---/    Nest Zone   = [CColor::GRAY80] reading value is approximately 0.80
 *                      Pheromone   = [CColor::GRAY40] reading value is approximately 0.40
 *
 * This function is used to determine if the robot is currently on top of a food item.
 *
 * @return     bool     "true" if the robot is currently on top of a food item,
 *                      "false" if the robot is not currently on top of a food item
 */
bool iAnt_controller::IsFindingFood() {
    /*
	// Obtain the current ground sensor readings for this controller object.
	const CCI_FootBotMotorGroundSensor::TReadings& groundReadings = groundSensor->GetReadings();
	// The ideal value is 0.0, but we must account for sensor read errors (+/- 0.1).
	CRange<Real> foodSensorRange(-0.1, 0.1);
	// Assign the ground readings to temporary variables for clarity.
	Real frontRightWheelReading = groundReadings[0].Value;
	Real frontLeftWheelReading  = groundReadings[1].Value;
	Real backLeftWheelReading   = groundReadings[2].Value;
	Real backRightWheelReading  = groundReadings[3].Value;

	// Pick up a food item if ANY of the four sensors is detecting it.
	if(foodSensorRange.WithinMinBoundIncludedMaxBoundIncluded(frontRightWheelReading) ||
	   foodSensorRange.WithinMinBoundIncludedMaxBoundIncluded(frontLeftWheelReading ) ||
	   foodSensorRange.WithinMinBoundIncludedMaxBoundIncluded(backLeftWheelReading  ) ||
	   foodSensorRange.WithinMinBoundIncludedMaxBoundIncluded(backRightWheelReading )) {
	    return true; // found food
	} */

    Real dts = (distanceTolerance * distanceTolerance);

    for(int i = 0; i < foodPositions.size(); i++) {
        if((position - foodPositions[i]).SquareLength() < dts) {
            return true; // robot is in the nest
        }
    }

	return false; // has not found food
}

// Is the robot holding a food item?
bool iAnt_controller::IsHoldingFood() {
	return holdingFood;
}

// the robot is picking up a food item
void iAnt_controller::PickupFood() {
	holdingFood = true;
}

// the robot is dropping off a food item
void iAnt_controller::DropOffFood() {
	holdingFood = false;
}

// update the list of available food positions
void iAnt_controller::UpdateFoodList(vector<CVector2> newFoodPositions) {
	foodPositions = newFoodPositions;
}

// update pheromone positions
void iAnt_controller::UpdatePheromoneList(vector<CVector2> newPheromonePositions) {
    pheromonePositions = newPheromonePositions;
}

// update fidelity positions
void iAnt_controller::UpdateFidelityList(vector<CVector2> newFidelityPositions) {
    fidelityPositions = newFidelityPositions;
}
// update the iAnt's position
void iAnt_controller::UpdatePosition(CVector2 newPosition) {
	position = newPosition;
}

// update simulation time
void iAnt_controller::UpdateTime(long int newTime) {
	simTime = newTime;
}

// return the robot's position
CVector2 iAnt_controller::Position() {
	return position;
}

// return the robot's fidelity position
CVector2 iAnt_controller::FidelityPosition() {
    return fidelityPosition;
}

// set new nest position
void iAnt_controller::SetNestPosition(CVector2 np) {
    nestPosition = np;
}

void iAnt_controller::SetNestRadiusSquared(Real r) {
    nestRadiusSquared = r;
}

// set squaqred radius of food items
void iAnt_controller::SetFoodRadiusSquared(Real rs) {
    foodRadiusSquared = rs;
}

// update pheromone
void iAnt_controller::SetTargetPheromone(iAnt_pheromone p)
{
	targetPheromone.Set(p);
}

// setup forage boundary
void iAnt_controller::SetForageRange(CRange<Real> X, CRange<Real> Y) {
    forageRangeX = X;
    forageRangeY = Y;
}

// get nest position
CVector2 iAnt_controller::GetNestPosition() {
    return nestPosition;
}

// get nest radius
Real iAnt_controller::GetNestRadius() {
    return sqrt(nestRadiusSquared);
}

// get food radius
Real iAnt_controller::GetFoodRadius() {
    return sqrt(foodRadiusSquared);
}

// get food position list from robot
vector<CVector2> iAnt_controller::GetFoodPositions() {
    return foodPositions;
}

// get pheromone position list from robot
vector<CVector2> iAnt_controller::GetPheromonePositions() {
    return pheromonePositions;
}

// get fidelity position list
vector<CVector2> iAnt_controller::GetFidelityPositions() {
    return fidelityPositions;
}

// get pheromone for master pheromone list in loop_functions
iAnt_pheromone iAnt_controller::GetTargetPheromone() {
	return sharedPheromone;
}

// return pheromoneDecayRate
Real iAnt_controller::PheromoneDecayRate() {
    return pheromoneDecayRate;
}

/* iAnt_controller Control Step Function
 *
 * This function is the primary integration between this class and the ARGoS simulator. ARGoS will
 * call this function once per frame or tick for each currently active robot using this controller.
 * The control scheme for this controller is modeled after a state machine, but it is not a true
 * state machine do to various modifications.
 */
void iAnt_controller::ControlStep() {
    if(foodPositions.size() == 0 && !IsHoldingFood()) {
		target = setPositionInBounds(nestPosition);

        if((position - target).SquareLength() < distanceTolerance) {
            steeringActuator->SetLinearVelocity(0.0, 0.0);
        } else {
            SetWheelSpeed();
        }
    } else {
	    switch(CPFA) {
		    case INACTIVE:
		    	inactive();
		    	break;
		    case DEPARTING:
		    	departing();
		    	break;
		    case SEARCHING:
		    	searching();
		    	break;
		    case RETURNING:
		    	returning();
	    }
    }
}

/* iAnt_controller Reset Function
 *
 * This function is called when the simulation user presses the reset button on the ARGoS simulator
 * GUI. Most variables in this controller are automatically set or only need to be set once except
 * for the ones in this reset list which are reset to default initialized values.
 */
void iAnt_controller::Reset() {
	// TODO make sure this reset function is actually resetting "EVERYTHING" it needs to...

	// Restart the simulation with the CPFA in the REST state.
	CPFA = INACTIVE;

	// Reset food data for this controller.
	holdingFood = false;
}

/* iAnt_controller Destroy Function
 *
 * This function is called after the user exits the simulation GUI. Any file read/write objects
 * or other objects you must delete or otherwise terminate are to be closed or deleted here. This
 * function operates similarly to a destructor function and undoes whatever was done by the call
 * to the iAnt_controller::Init(TConfigurationNode& node) function. Currently unimplemented and
 * unnecessary. This function is kept here for completeness and possible future expansion of the
 * iAnt_controller class.
 */
void iAnt_controller::Destroy() {
	// not in use
}

void iAnt_controller::inactive() {
	SetRandomSearchLocation();
	CPFA = DEPARTING;
}

void iAnt_controller::departing() {
    if(IsHoldingFood() == true) {
		if(IsFindingFood() == true) senseLocalResourceDensity();
		target = setPositionInBounds(nestPosition);
		CPFA = RETURNING;
	} else if(informed == false) {
		if(RNG->Uniform(CRange<Real>(0.0, 1.0)) < travelGiveupProbability) {
			searchTime = 0;
			CPFA = SEARCHING;
		}
	} else if((position - target).SquareLength() < distanceTolerance) {
		searchTime = 0;
		CPFA = SEARCHING;
		informed = false;
	}

	SetWheelSpeed();
}

void iAnt_controller::searching() {
	if(IsHoldingFood() == false) {
		if(RNG->Uniform(CRange<Real>(0.0, 1.0)) < searchGiveupProbability) {
			target = setPositionInBounds(nestPosition);
			CPFA = RETURNING;
		}
		else if((simTime % 8 == 0) && (position - target).SquareLength() < distanceTolerance) {
			if(informed == false) {
				// Get a random rotation angle and then add it to the getVectorToLight angle. This serves the functionality
				// of a compass and causes the rotation to be relative to the robot's current direction.
				CRadians rotation(RNG->Gaussian(uninformedSearchCorrelation.GetValue())),
						 angle(rotation.UnsignedNormalize() + RobotHeading().UnsignedNormalize());

				target = setPositionInBounds(CVector2(searchStepSize, angle) + position);
			}
			else {
				float correlation = exponentialDecay((CRadians::TWO_PI).GetValue(), searchTime++, informedSearchDecay);
				CRadians rotation(bound(correlation, -(CRadians::PI).GetValue(), (CRadians::PI).GetValue())),
						 angle(rotation.UnsignedNormalize() + RobotHeading().UnsignedNormalize());

				target = setPositionInBounds(CVector2(searchStepSize, angle) + position);
			}
		}
	} else {
		if(IsFindingFood() == true) senseLocalResourceDensity();
		target = setPositionInBounds(nestPosition);
		CPFA = RETURNING;
	}

	SetWheelSpeed();
}

void iAnt_controller::returning() {
	if((position - target).SquareLength() < distanceTolerance) {
		if(poissonCDF(resourceDensity, pheromoneRate) > RNG->Uniform(CRange<Real>(0.0, 1.0))) {
			sharedPheromone.Set(iAnt_pheromone(fidelityPosition, simTime, pheromoneDecayRate));
		}

		if(poissonCDF(resourceDensity, siteFidelityRate) > RNG->Uniform(CRange<Real>(0.0, 1.0))) {
			target = setPositionInBounds(fidelityPosition);
			informed = true;
		}
		else if(targetPheromone.IsActive() == true) {
			target = setPositionInBounds(targetPheromone.Location());
			informed = true;
		}
		else {
			informed = false;
			SetRandomSearchLocation();
		}

		CPFA = DEPARTING;
	}
	else SetWheelSpeed();
}

void iAnt_controller::senseLocalResourceDensity()
{
	resourceDensity = 0; // DO count the food item robot just found

	for(size_t i = 0; i < foodPositions.size(); i++) {
		if((position - foodPositions[i]).SquareLength() < searchRadiusSquared) {
			resourceDensity++;
		}

        // clean location
        //if((position - foodPositions[i]).SquareLength() < foodRadiusSquared) {
            //fidelityPosition = foodPositions[i];
        //}
	}

    // messy location
    fidelityPosition = position;
}

CVector2 iAnt_controller::setPositionInBounds(CVector2 p) {
	if(p.GetX() > forageRangeX.GetMax()) p.SetX(forageRangeX.GetMax());
	else if(p.GetX() < forageRangeX.GetMin()) p.SetX(forageRangeX.GetMin());

	if(p.GetY() > forageRangeY.GetMax()) p.SetY(forageRangeY.GetMax());
	else if(p.GetY() < forageRangeY.GetMin()) p.SetY(forageRangeY.GetMin());

	return p;
}

// set target to a random location
void iAnt_controller::SetRandomSearchLocation() {
	// randomly set the target somewhere in the arena
	target.SetX(RNG->Uniform(forageRangeX));
	target.SetY(RNG->Uniform(forageRangeY));

    target = setPositionInBounds(target);
}

bool iAnt_controller::CollisionDetection() {
	const CCI_FootBotProximitySensor::TReadings& proximityReadings = proximitySensor->GetReadings();
	size_t collisionsDetected = 0;

	for(size_t i = 0; i < proximityReadings.size(); i++) {
		if((proximityReadings[i].Value > 0.0) &&
           (angleTolerance.WithinMinBoundIncludedMaxBoundIncluded(proximityReadings[i].Angle))) {
            collisionsDetected++;
		}
	}

	return (collisionsDetected > 0) ? (true) : (false);
}

/*
 * ADD to this heading to turn LEFT
 * SUBTRACT to this heading to turn RIGHT
 *
 * this reading will give a value:
 *
 * +     0 degrees [north],
 * -    90 degrees [east],
 * +/- 180 degrees [south],
 * +    90 degrees [west]
 */
CRadians iAnt_controller::RobotHeading() {
    const CCI_PositioningSensor::SReading& sReading = compassSensor->GetReading();
    CQuaternion orientation = sReading.Orientation;

    /* Convert quaternion to euler */
    CRadians z_angle, y_angle, x_angle;
    orientation.ToEulerAngles(z_angle, y_angle, x_angle);

    /* Angle to z-axis represents compass heading */
    return z_angle;
}

void iAnt_controller::SetWheelSpeed() {
	CRadians heading = (RobotHeading() - (target - position).Angle()).SignedNormalize();

	if(CollisionDetection() == true) {
		// 32 = 2 seconds, 16 frames per second (set in XML) by 2
		collisionDelay = simTime + 32;
		/* turn left */
		steeringActuator->SetLinearVelocity(-maxSpeed, maxSpeed);
	} else if((heading <= angleTolerance.GetMin()) && (collisionDelay < simTime)) {
		/* turn left */
		steeringActuator->SetLinearVelocity(-maxSpeed, maxSpeed);
	} else if((heading >= angleTolerance.GetMax()) && (collisionDelay < simTime)) {
		/* turn right */
		steeringActuator->SetLinearVelocity(maxSpeed, -maxSpeed);
	} else {
		/* go straight */
		steeringActuator->SetLinearVelocity(maxSpeed, maxSpeed);
	}
}

REGISTER_CONTROLLER(iAnt_controller, "iAnt_controller")

package edu.unm;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.*;

/**
 * @author John Ericksen
 */
public class Experiment {

    private final Logger log;
    private final List<OrganismExecutor> executors;
    private final int populationSize;
    private final int runtime;
    private final int distribution;
    private final int entityCount;

    public Experiment(List<OrganismExecutor> executors, Logger log, int populationSize, int runtime, int distribution, int entityCount) {
        this.executors = executors;
        this.log = log;
        this.populationSize = populationSize;
        this.runtime = runtime;
        this.distribution = distribution;
        this.entityCount = entityCount;
    }

    public void run() throws Exception {
        long start = System.currentTimeMillis();

        //Startup population
        List<Chromosome> population = new ArrayList<Chromosome>();

        for (int i = 0; i < 10; i++){
            population.add(new Chromosome());
        }

        for (int e = 0; e < 100; e++) {

            long epochStart = System.currentTimeMillis();
            log.log("Epoch " + e);

            final BlockingQueue<OrganismIdWrapper> queue = new ArrayBlockingQueue<OrganismIdWrapper>(populationSize);

            for (int i = 0; i < population.size(); i++) {
                queue.add(new OrganismIdWrapper(population.get(i), i));
            }

            ExecutorService executor = Executors.newFixedThreadPool(executors.size());

            final int epoch = e;
            for (final OrganismExecutor organismExecutor : executors) {
                executor.execute(new Runnable() {
                    @Override
                    public void run() {
                        organismExecutor.listen(queue, epoch, runtime, distribution, entityCount);
                    }
                });
            }

            executor.shutdown();

            while (!executor.awaitTermination(1, TimeUnit.SECONDS)) {}

            //run GA

            log.log("Epoch " + e + " took " + (System.currentTimeMillis() - epochStart) + "ms");
        }

        log.log("Finished: " + (System.currentTimeMillis() - start) + "ms");
    }
}

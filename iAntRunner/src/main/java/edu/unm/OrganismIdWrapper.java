package edu.unm;

/**
 * @author John Ericksen
 */
public class OrganismIdWrapper {

    private final Chromosome organism;
    private final int id;

    public OrganismIdWrapper(Chromosome organism, int id) {
        this.organism = organism;
        this.id = id;
    }

    public Chromosome getOrganism() {
        return organism;
    }

    public int getId() {
        return id;
    }

    public String buildChromosone() {
        StringBuilder builder = new StringBuilder();

        //todo: build chromosome string
        builder.append("cromosomedata");

        return builder.toString();
    }
}

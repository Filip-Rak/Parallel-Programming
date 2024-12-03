// -------------------
// Separate thread for each ASCII sign
// -------------------

public class ThreadVariant1 extends Thread
{

    // Attributes
    private final int thread_id;
    private final char symbol;
    private final Image image_ref;
    private int count;

    // Constructor
    public ThreadVariant1(int thread_id, char symbol, Image image_ref)
    {
        this.thread_id = thread_id;
        this.symbol = symbol;
        this.image_ref = image_ref;
        this.count = 0;
    }

    // Thread execution method
    public void run()
    {
        // Get the count
        count = image_ref.get_symbol_count(symbol);

        // Display the count in a synchronised manner
        image_ref.display_symbol_count(thread_id, symbol, count);

        // Save the count in Image class
        image_ref.add_count_to_histogram(symbol, count);
    }
}

public class ThreadVariant2 implements Runnable
{
    // Attributes
    private final int start_index;
    private final int end_index;
    private final int index_stride;
    private final Image image_ref;
    private final int thread_id;

    // Constructor
    public ThreadVariant2(int start_index, int end_index, int index_stride, Image image_ref, int thread_id)
    {
        this.start_index = start_index;
        this.end_index = end_index;
        this.index_stride = index_stride;
        this.image_ref = image_ref;
        this.thread_id = thread_id;
    }

    // Thread execution method
    public void run()
    {
        // Fill parallel result
        image_ref.calc_histogram_symbol(start_index, end_index, index_stride);

        // Display result on screen
        for (int i = start_index; i < end_index; i += index_stride)
        {
            char symbol = (char) (i + Image.get_begin_index());
            image_ref.display_symbol_count(thread_id, symbol);
        }
    }
}

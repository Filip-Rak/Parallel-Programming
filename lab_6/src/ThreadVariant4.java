// -------------------
// Decomposition for character matrix
// -------------------

import java.util.concurrent.Callable;

public class ThreadVariant4 implements Callable<int[]>
{
    // Attributes
    // ---------------

    // Rows
    private final int row_start;
    private final int row_end;
    private final int row_stride;

    // Columns
    private final int col_start;
    private final int col_end;
    private final int col_stride;

    // References
    Image image_ref;

    // Output
    int[] local_histogram;

    // Constructor
    // ---------------
    public ThreadVariant4(int row_start, int row_end, int row_stride,
                          int col_start, int col_end, int col_stride, Image image_ref)
    {
        // Row initialization
        this.row_start = row_start;
        this.row_end = row_end;
        this.row_stride = row_stride;

        // Column initializations
        this.col_start = col_start;
        this.col_end = col_end;
        this.col_stride = col_stride;

        // Save reference
        this.image_ref = image_ref;

        // Initialize local array for output storage
        int size = Image.get_symbol_count();
        local_histogram = new int[size];
        for (int i = 0; i < size; i++)
            local_histogram[i] = 0;

    }

    // Public Methods
    // ---------------
    public int[] call()
    {
        image_ref.calc_histogram_locally_on_thread(row_start, row_end, row_stride, col_start, col_end, col_stride, local_histogram);
        return local_histogram;
    }
}

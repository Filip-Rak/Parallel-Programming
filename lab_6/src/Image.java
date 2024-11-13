import java.util.Arrays;
import  java.util.Random;

public class Image
{
    // Attributes
    // ---------------
    private static final int SYMBOL_COUNT = 94;
    private static final int BEGIN_INDEX = 33;

    private final int size_n;
    private final int size_m;
    private final char[][] arr;
    private final char[] arr_symbols;
    private final int[] histogram;
    private final int[] histogram_parrarel;

    // Constructor
    // ---------------
    public Image(int n, int m)
    {
        this.size_n = n;
        this.size_m = m;
        arr = new char[n][m];
        arr_symbols = new char[SYMBOL_COUNT];

        final Random random = new Random();

        // For general case where symbols could not be just integers
        for(int k=0;k<SYMBOL_COUNT;k++)
        {
            arr_symbols[k] = (char)(k+BEGIN_INDEX); // Substitute symbols
        }

        // Fill with random ascii
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<m;j++)
            {
                arr[i][j] = arr_symbols[random.nextInt(SYMBOL_COUNT)];  // ascii BEGIN_INDEX-127
                //tab[i][j] = (char)(random.nextInt(SYMBOL_COUNT)+BEGIN_INDEX);  // ascii BEGIN_INDEX-127
                System.out.print(arr[i][j] + " ");
            }
            System.out.print("\n");
        }
        System.out.print("\n\n");

        // Initialize histogram
        histogram = new int[SYMBOL_COUNT];
        clear_histogram();

        // Make a separate array for comparisons
        histogram_parrarel = new int[SYMBOL_COUNT];
        System.arraycopy(histogram, 0, histogram_parrarel, 0, SYMBOL_COUNT);
    }

    // Public Methods
    // ---------------
    public void clear_histogram()
    {
        for(int i = 0; i < SYMBOL_COUNT; i++)
            histogram[i] = 0;
    }

    public void clear_parallel_histogram()
    {
        for(int i = 0; i < SYMBOL_COUNT; i++)
            histogram_parrarel[i] = 0;
    }

    public void calculate_histogram()
    {
        for(int i = 0; i < size_n; i++)
        {
            for(int j = 0; j < size_m; j++)
            {
                for(int k = 0; k < SYMBOL_COUNT; k++)
                {
                    if(arr[i][j] == arr_symbols[k])
                        histogram[k]++;
                }
            }
        }
    }

    public int get_symbol_count(char symbol_to_find)
    {
        int counter = 0;
        for (int i = 0; i < size_n; i++)
        {
            for (int j = 0; j < size_m; j++)
            {
                if (arr[i][j] == symbol_to_find)
                    counter++;
            }
        }

        return counter;
    }

    public synchronized void display_symbol_count(long thread_id, char symbol, int count)
    {
        // Abort printing if no occurrences
        if (count <= 0)
            return;

        // Print the count
        System.out.print("Thread: " + thread_id + ": |" + symbol + "| [" + count + "]: ");
        for (int i = 0; i < count; i++)
            System.out.print("=");

        System.out.println();
    }

    public synchronized void display_symbol_count(long thread_id, char symbol)
    {
        int index = (int)symbol - BEGIN_INDEX;
        int count = histogram_parrarel[index];

        display_symbol_count(thread_id, symbol, count);
    }

    public void add_count_to_histogram(char symbol, int count)
    {
        int index = (int)symbol - BEGIN_INDEX;
        histogram_parrarel[index] = count;
    }

    public void print_histogram()
    {
        for(int i = 0; i < SYMBOL_COUNT;i++)
        {
            System.out.print(arr_symbols[i] + " " + histogram[i] + "\n");
        }
    }

    public void calc_histogram(int start_row, int end_row, int row_stride,
                               int start_col, int end_col, int col_stride,
                               int symbol_start, int symbol_end, int symbol_stride)
    {
        for (int row = start_row; row < end_row; row += row_stride)
        {
            for (int col = start_col; col < end_col; col += col_stride)
            {
                for (int symbol = symbol_start; symbol < symbol_end; symbol += symbol_stride)
                {
                    if (arr[row][col] == arr_symbols[symbol])
                        histogram_parrarel[symbol] += 1;
                }
            }
        }
    }

    public void calc_histogram_symbol(int symbol_start, int symbol_end, int symbol_stride)
    {
        this.calc_histogram(0, size_n, 1, 0, size_m, 1, symbol_start, symbol_end, symbol_stride);
    }

    public boolean verify_thread_result()
    {
        return Arrays.equals(histogram, histogram_parrarel);
    }


    // Static Methods
    // ---------------
    public static int get_symbol_count()
    {
        return SYMBOL_COUNT;
    }

    public static int get_begin_index()
    {
        return BEGIN_INDEX;
    }
}

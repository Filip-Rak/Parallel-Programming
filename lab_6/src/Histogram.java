import java.util.Scanner;


class Histogram
{
    private static boolean all_clear = true;

    public static void main(String[] args)
    {
        // Get size and create a histogram
        Scanner scanner = new Scanner(System.in);
        System.out.println("Image size: rows, columns");
        int input_rows = scanner.nextInt();
        int input_cols = scanner.nextInt();

        Image image_1 = new Image(input_rows, input_cols);
        image_1.calculate_histogram();
        // image_1.print_histogram();

        // Common thread arguments
        int ascii_start_index = Image.get_begin_index();
        int ascii_length = Image.get_symbol_count();
        int ascii_end_index = Image.get_begin_index() + ascii_length;

        /* Thread Variant 1 - Each ascii sign gets a thread */
        System.out.println("---------- Thread Variant 1 ----------");

        // Create and start threads
        ThreadVariant1[] threads_v1 = new ThreadVariant1[ascii_length];
        int[] ids = new int[ascii_length];

        for (int i = 0; i < ascii_length; i++)
        {
            char symbol = (char)(ascii_start_index + i);
            ids[i] = i;

            threads_v1[i] = new ThreadVariant1(ids[i], symbol, image_1);
            threads_v1[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(threads_v1);

        // Verify the result
        verify_and_clear(image_1);

        // Common arguments
        final int THREAD_NUM = 12;

        /* Thread Variant 2 - 1D Block Decomposition of ASCII */
        System.out.println("---------- Thread Variant 2 ----------");

        // Arrays for keeping track o threads
        ThreadVariant2[] threads_v2 = new ThreadVariant2[THREAD_NUM];
        Thread[] thread_instances = new Thread[THREAD_NUM];
        ids = new int[THREAD_NUM];

        // Create runnable classes and engage threads
        for (int i = 0; i < THREAD_NUM; i++)
        {
            // Block decomposition
            int per_thread = (ascii_length + THREAD_NUM - 1) / THREAD_NUM;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, ascii_length);
            int index_stride = 1;
            ids[i] = i;

            // Create runnable class instance
            threads_v2[i] = new ThreadVariant2(start_index, end_index, index_stride, image_1, ids[i]);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v2[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Verify the result
        verify_and_clear(image_1);

        /* Thread Variant 3_1 - Cyclical row decomposition of existing characters */
        System.out.println("---------- Thread Variant 3_1 ----------");
        ThreadVariant3[] threads_v3 = new ThreadVariant3[THREAD_NUM];

        // Decomposition and thread creation
        for (int i = 0; i < THREAD_NUM; i++)
        {
            // Cyclical row decomposition
            int start_index = i;
            int end_index = input_rows;
            int stride = THREAD_NUM;

            // create runnable class instance
            threads_v3[i] = new ThreadVariant3(start_index, end_index, stride, 0, input_cols, 1, image_1);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < THREAD_NUM; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_1.include_histogram(result);
        }

        // Display result
        image_1.display_symbol_count();

        // Verify the result
        verify_and_clear(image_1);

        /* Thread Variant 3_2 - Block column decomposition of existing characters */
        System.out.println("---------- Thread Variant 3_2 ----------");
        threads_v3 = new ThreadVariant3[THREAD_NUM];

        // Decomposition and thread creation
        for (int i = 0; i < THREAD_NUM; i++)
        {
            // Block column decomposition
            int per_thread = (input_cols + THREAD_NUM - 1) / THREAD_NUM;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, input_cols);
            int index_stride = 1;

            // create runnable class instance
            threads_v3[i] = new ThreadVariant3(0, input_rows , 1, start_index, end_index, index_stride, image_1);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < THREAD_NUM; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_1.include_histogram(result);
        }

        // Display result
        image_1.display_symbol_count();

        // Verify the result
        verify_and_clear(image_1);

        /* Thread Variant 3_3 - 2D decomposition of existing characters */
        System.out.println("---------- Thread Variant 3_3 ----------");
        threads_v3 = new ThreadVariant3[THREAD_NUM];

        // Determine thread grid dimensions
        int grid_size = (int) Math.ceil(Math.sqrt(THREAD_NUM));

        // Decomposition and thread creation
        for (int i = 0; i < THREAD_NUM; i++)
        {
            // Determine row and column position in the grid
            int thread_row = i / grid_size;
            int thread_col = i % grid_size;

            // Divide rows among grid rows
            int row_per_thread = (input_rows + grid_size - 1) / grid_size;
            int row_start = thread_row * row_per_thread;
            int row_end = Math.min((thread_row + 1) * row_per_thread, input_rows);

            // Divide columns among grid columns
            int col_per_thread = (input_cols + grid_size - 1) / grid_size;
            int col_start = thread_col * col_per_thread;
            int col_end = Math.min((thread_col + 1) * col_per_thread, input_cols);

            // Create runnable class instance for 2D block
            threads_v3[i] = new ThreadVariant3(row_start, row_end, 1, col_start, col_end, 1, image_1);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < THREAD_NUM; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_1.include_histogram(result);
        }

        // Display result
        image_1.display_symbol_count();

        // Verify the result
        verify_and_clear(image_1);


        // Debug to check if all results are fine
        System.out.println("------------------------------");
        System.out.println("All clear: " + all_clear);

    }
    public static void wait_for_threads(Thread[] threads)
    {
        for (int i = 0; i < threads.length; i++)
        {
            try
            {
                threads[i].join();
            }
            catch (InterruptedException exc)
            {
                System.out.println("Exception with thread: " + i);
            }
        }
    }

    public static void verify_and_clear(Image image)
    {
        // Verification and output
        boolean success = image.verify_thread_result();
        System.out.println("Results are correct: " + success);
        image.clear_parallel_histogram();

        // Flag for all test success
        if(!success)
            all_clear = false;
    }

}


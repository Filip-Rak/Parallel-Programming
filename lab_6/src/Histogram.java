import java.util.Scanner;


class Histogram
{
    private static boolean all_clear = true;

    public static void main(String[] args)
    {
        // --------------- Initial sequential histogram ---------------

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

        // Common arguments
        final int THREAD_NUM = 12;

        // --------------- Exercises ---------------

        // Thread Variant 1 - Each ascii sign gets a thread
        ex1(ascii_length, ascii_start_index, image_1);

        // Thread Variant 2 - 1D Block Decomposition of ASCII
        ex2(ascii_length, THREAD_NUM, image_1);

        // Thread Variant 3_1 - Cyclical row decomposition of existing characters
        ex3(THREAD_NUM, input_rows, input_cols, image_1);

        // Thread Variant 3_2 - Block column decomposition of existing characters
        ex4(THREAD_NUM, input_cols, input_rows, image_1);

        // Thread Variant 3_3 - 2D decomposition of existing characters
        ex5(THREAD_NUM, input_rows, input_cols, image_1);

        // --------------- Debug if fails occurred ---------------
        System.out.println("------------------------------");
        System.out.println("All clear: " + all_clear);

    }

    public static void ex1(int ascii_length, int ascii_start_index, Image image_ref)
    {
        System.out.println("---------- Thread Variant 1 ----------");

        // Create and start threads
        ThreadVariant1[] threads_v1 = new ThreadVariant1[ascii_length];
        int[] ids = new int[ascii_length];

        for (int i = 0; i < ascii_length; i++)
        {
            char symbol = (char)(ascii_start_index + i);
            ids[i] = i;

            threads_v1[i] = new ThreadVariant1(ids[i], symbol, image_ref);
            threads_v1[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(threads_v1);

        // Verify the result
        verify_and_clear(image_ref);
    }

    public static void ex2(int ascii_length, int thread_num, Image image_ref)
    {
        System.out.println("---------- Thread Variant 2 ----------");

        // Arrays for keeping track o threads
        ThreadVariant2[] threads_v2 = new ThreadVariant2[thread_num];
        Thread[] thread_instances = new Thread[thread_num];
        int[] ids = new int[thread_num];

        // Create runnable classes and engage threads
        for (int i = 0; i < thread_num; i++)
        {
            // Block decomposition
            int per_thread = (ascii_length + thread_num - 1) / thread_num;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, ascii_length);
            int index_stride = 1;
            ids[i] = i;

            // Create runnable class instance
            threads_v2[i] = new ThreadVariant2(start_index, end_index, index_stride, image_ref, ids[i]);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v2[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Verify the result
        verify_and_clear(image_ref);
    }

    public static void ex3(int thread_num, int input_rows, int input_cols, Image image_ref)
    {
        System.out.println("---------- Thread Variant 3_1 ----------");
        ThreadVariant3[] threads_v3 = new ThreadVariant3[thread_num];
        Thread[] thread_instances = new Thread[thread_num];

        // Decomposition and thread creation
        for (int i = 0; i < thread_num; i++)
        {
            // Cyclical row decomposition
            int start_index = i;
            int end_index = input_rows;
            int stride = thread_num;

            // create runnable class instance
            threads_v3[i] = new ThreadVariant3(start_index, end_index, stride, 0, input_cols, 1, image_ref);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < thread_num; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_ref.include_histogram(result);
        }

        // Display result
        image_ref.display_symbol_count();

        // Verify the result
        verify_and_clear(image_ref);
    }

    public static void ex4(int thread_num, int input_cols, int input_rows, Image image_ref)
    {
        System.out.println("---------- Thread Variant 3_2 ----------");
        ThreadVariant3[] threads_v3 = new ThreadVariant3[thread_num];
        Thread[] thread_instances = new Thread[thread_num];

        // Decomposition and thread creation
        for (int i = 0; i < thread_num; i++)
        {
            // Block column decomposition
            int per_thread = (input_cols + thread_num - 1) / thread_num;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, input_cols);
            int index_stride = 1;

            // create runnable class instance
            threads_v3[i] = new ThreadVariant3(0, input_rows , 1, start_index, end_index, index_stride, image_ref);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < thread_num; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_ref.include_histogram(result);
        }

        // Display result
        image_ref.display_symbol_count();

        // Verify the result
        verify_and_clear(image_ref);
    }

    public static void ex5(int thread_num, int input_rows, int input_cols, Image image_ref)
    {
        System.out.println("---------- Thread Variant 3_3 ----------");
        ThreadVariant3[] threads_v3 = new ThreadVariant3[thread_num];
        Thread[] thread_instances = new Thread[thread_num];

        // Decomposition and thread creation
        for (int i = 0; i < thread_num; i++)
        {
            // Cyclic row decomposition
            int row_start = i;
            int row_end = input_rows;
            int row_stride = thread_num;

            // Cyclic column decomposition
            int col_start = i;
            int col_end = input_cols;
            int col_stride = thread_num;

            // Create runnable class instance for cyclic 2D decomposition
            threads_v3[i] = new ThreadVariant3(row_start, row_end, row_stride, col_start, col_end, col_stride, image_ref);

            // Create a thread instance and start it
            thread_instances[i] = new Thread(threads_v3[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < thread_num; i++)
        {
            int[] result = threads_v3[i].get_results();
            image_ref.include_histogram(result);
        }

        // Display result
        image_ref.display_symbol_count();

        // Verify the result
        verify_and_clear(image_ref);
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


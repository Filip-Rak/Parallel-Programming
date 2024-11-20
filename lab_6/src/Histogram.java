import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;


class Histogram
{
    private static boolean all_clear = true;

    public static void main(String[] args)
    {
        // ---------- Initial sequential histogram ----------

        // Get size and create a histogram
        Scanner scanner = new Scanner(System.in);
        System.out.print("Image size -> rows, columns: ");
        int input_rows = scanner.nextInt();
        int input_cols = scanner.nextInt();

        Image image = new Image(input_rows, input_cols);
        image.calculate_histogram();
        // image.print_histogram();

        // Common thread arguments
        final int ascii_start_index = Image.get_begin_index();
        final int ascii_length = Image.get_symbol_count();
        final int THREAD_NUM = 12;

        // ---------- Exercises ----------

        // Thread Variant 1 - Each ascii sign gets a thread
        ex1(ascii_length, ascii_start_index, image);

        // Thread Variant 2 - 1D Block Decomposition of ASCII
        ex2(ascii_length, THREAD_NUM, image);

        // Thread Variant 3_1 - Cyclical row decomposition of existing characters
        ex3(THREAD_NUM, input_rows, input_cols, image);

        // Thread Variant 3_2 - Block column decomposition of existing characters
        ex4(THREAD_NUM, input_cols, input_rows, image);

        // Thread Variant 3_3 - 2D decomposition of existing characters
        ex5(input_rows, input_cols, image);

        // Thread Variant 3_4 - Computation using Thread Pool
        ex6(THREAD_NUM, input_cols, input_rows, image);

        // ---------- Debug if fails occurred ----------
        System.out.println("------------------------------");
        System.out.println("All clear: " + all_clear);

    }

    // Exercise Methods
    // ---------------
    private static void ex1(int ascii_length, int ascii_start_index, Image image_ref)
    {
        System.out.println("---------- Thread Variant 1 ----------");

        // Create and start threads
        ThreadVariant1[] threads_v1 = new ThreadVariant1[ascii_length];

        for (int i = 0; i < ascii_length; i++)
        {
            char symbol = (char)(ascii_start_index + i);
            threads_v1[i] = new ThreadVariant1(i, symbol, image_ref);
            threads_v1[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(threads_v1);

        // Verify the result
        verify_and_clear(image_ref);
    }

    private static void ex2(int ascii_length, int thread_num, Image image_ref)
    {
        System.out.println("---------- Thread Variant 2 ----------");

        // Arrays for keeping track of threads
        ThreadVariant2[] threads_v2 = new ThreadVariant2[thread_num];
        Thread[] thread_instances = new Thread[thread_num];

        // Create runnable classes and engage threads
        for (int i = 0; i < thread_num; i++)
        {
            // Block decomposition
            int per_thread = (ascii_length + thread_num - 1) / thread_num;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, ascii_length);
            int index_stride = 1;

            // Create runnable class instance
            threads_v2[i] = new ThreadVariant2(start_index, end_index, index_stride, image_ref, i);

            // Create a thread instance and run it
            thread_instances[i] = new Thread(threads_v2[i]);
            thread_instances[i].start();
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Verify the result
        verify_and_clear(image_ref);
    }

    private static void ex3(int thread_num, int input_rows, int input_cols, Image image_ref)
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

    private static void ex4(int thread_num, int input_cols, int input_rows, Image image_ref)
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

            // Create runnable class instance
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

    private static void ex5(int input_rows, int input_cols, Image image_ref)
    {
        System.out.println("---------- Thread Variant 3_3 2D Grid Decomposition ----------");

        // Calculate the optimal number of threads based on matrix dimensions
        int target_threads = (int)Math.sqrt(input_rows * input_cols); // Start with the square root of total size
        target_threads = Math.max(1, target_threads); // Ensure at least 1 thread

        // Find the nearest smaller perfect square
        int sqrt_threads = (int)Math.sqrt(target_threads);
        int thread_num = sqrt_threads * sqrt_threads; // Ensure it's a perfect square

        // System.out.println("Calculated thread_num: " + thread_num + " (sqrt_threads: " + sqrt_threads + ")");

        // Compute rows and columns per block with round up
        int rows_per_block = (input_rows + sqrt_threads - 1) / sqrt_threads;
        int cols_per_block = (input_cols + sqrt_threads - 1) / sqrt_threads;

        // Allocate memory for threads
        ThreadVariant3[] threads_v3 = new ThreadVariant3[thread_num];
        Thread[] thread_instances = new Thread[thread_num];

        int thread_id = 0;

        // Create threads for a 2D grid
        for (int row_block = 0; row_block < sqrt_threads; row_block++)
        {
            for (int col_block = 0; col_block < sqrt_threads; col_block++)
            {
                // Calculate the range of rows and columns for each thread
                int row_start = row_block * rows_per_block;
                int row_end = Math.min(row_start + rows_per_block, input_rows);
                int col_start = col_block * cols_per_block;
                int col_end = Math.min(col_start + cols_per_block, input_cols);

                // Skip threads with no valid range
                if (row_start >= row_end || col_start >= col_end)
                {
                    System.out.printf("Thread: %d has no valid range to process. Skipping...\n", thread_id);
                    thread_id++;
                    continue;
                }

                // Create runnable class instance for this thread
                threads_v3[thread_id] = new ThreadVariant3(row_start, row_end, 1, col_start, col_end, 1, image_ref);

                // Log ranges for debugging
                /* System.out.println("Thread " + thread_id + " processing rows: " +
                        row_start + " to " + (row_end - 1) + " and columns: " +
                        col_start + " to " + (col_end - 1)); */

                // Create and start the thread
                thread_instances[thread_id] = new Thread(threads_v3[thread_id]);
                thread_instances[thread_id].start();
                thread_id++;
            }
        }

        // Wait for threads to finish
        wait_for_threads(thread_instances);

        // Consolidate thread results
        for (int i = 0; i < thread_num; i++)
        {
            if (threads_v3[i] != null)  // Avoid skipped threads
            {
                int[] result = threads_v3[i].get_results();
                image_ref.include_histogram(result);
            }
        }

        // Display result
        image_ref.display_symbol_count();

        // Verify the result
        verify_and_clear(image_ref);
    }

    private static void ex6(int thread_num, int input_cols, int input_rows, Image image_ref)
    {
        System.out.println("---------- Thread Variant 3_4 Thread Pool ----------");
        ThreadVariant3[] tasks = new ThreadVariant3[thread_num];
        ExecutorService executor = Executors.newFixedThreadPool(thread_num);

        // Decomposition and thread creation
        for (int i = 0; i < thread_num; i++)
        {
            // Block column decomposition
            int per_thread = (input_cols + thread_num - 1) / thread_num;
            int start_index = (i * per_thread);
            int end_index = Math.min((i + 1) * per_thread, input_cols);
            int index_stride = 1;

            // Create runnable class instance
            tasks[i] = new ThreadVariant3(0, input_rows , 1, start_index, end_index, index_stride, image_ref);
            executor.execute(tasks[i]);
        }

        // Wait for threads to finish execution
        executor.shutdown();
        try
        {
            boolean ignore =  executor.awaitTermination(1, TimeUnit.MINUTES);
        }
        catch (InterruptedException exception)
        {
            System.out.println("Exception: " + exception);
        }

        // Consolidate thread results
        for (int i = 0; i < thread_num; i++)
        {
            int[] result = tasks[i].get_results();
            image_ref.include_histogram(result);
        }

        // Display result
        image_ref.display_symbol_count();

        // Verify the result
        verify_and_clear(image_ref);
    }
    // Helper Methods
    // ---------------
    private static void wait_for_threads(Thread[] threads)
    {
        for (int i = 0; i < threads.length; i++)
        {
            // Skip uninitialized threads
            if (threads[i] == null)
                continue;

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

    private static void verify_and_clear(Image image)
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


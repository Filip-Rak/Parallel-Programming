import java.util.Scanner;


class Histogram
{

    public static void main(String[] args)
    {
        // Get size and create a histogram
        Scanner scanner = new Scanner(System.in);
        System.out.println("Image size: rows, columns");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        Image image_1 = new Image(n, m);
        image_1.calculate_histogram();
        // image_1.print_histogram();

        // Common thread arguments
        int ascii_start_index = Image.get_begin_index();
        int ascii_length = Image.get_symbol_count();
        int ascii_end_index = Image.get_begin_index() + ascii_length;

        // Thread Variant 1 - Each ascii sign gets a thread
        System.out.println("---------- Thread Variant 1 ----------");

        // Create and start threads
        ThreadVariant1[] threads_v1 = new ThreadVariant1[ascii_length];
        for (int i = 0; i < ascii_length; i++)
        {
            char symbol = (char)(ascii_start_index + i);
            threads_v1[i] = new ThreadVariant1(i, symbol, image_1);
            threads_v1[i].start();
        }

        // Wait for threads to finish
        for(int i = 0; i < ascii_length; i++)
        {
            try
            {
                threads_v1[i].join();
            }
            catch (InterruptedException exc)
            {
                System.out.println("Exception with thread: " + i);
            }
        }

        // Verify the result
        System.out.println("Results are correct: " + image_1.verify_thread_result());
        image_1.clear_parallel_histogram();

        // Common arguments
        final int THREAD_NUM = 12;

        // Thread Variant 2 - 1D Block Decomposition of ASCII
        System.out.println("---------- Thread Variant 2 ----------");

        // Create and engage threads
        ThreadVariant2[] threads_v2 = new ThreadVariant2[THREAD_NUM];
        for (int i = 0; i < THREAD_NUM; i++)
        {
            int per_thread = (ascii_length + THREAD_NUM - 1) / THREAD_NUM;
            int start_index = (i * per_thread);
            int end_index = (i == THREAD_NUM - 1) ? THREAD_NUM : (i + 1) * per_thread;
            int index_stride = 1;

            threads_v2[i] = new ThreadVariant2(start_index, end_index, index_stride, image_1, i);
        }

    }
}


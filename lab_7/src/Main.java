import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.*;

public class Main
{
    // Main Function
    // ---------------
    public static void main(String[] args)
    {
        // Common arguments
        double x_start = 0.0;
        double x_end = Math.PI;
        double delta_x = 0.1;
        int thread_num = 6;

        // Read user input
        Scanner input = new Scanner(System.in);
        System.out.print("dx: ");
        delta_x = input.nextDouble();

        // Sequential integral calculation
        ex1(x_start, x_end, delta_x);

        // Parallel integral computation with thread pool
        ex2(x_start, x_end, delta_x, thread_num);

        // Parallel integral computation with Runnable interface
        ex4(x_start, x_end, delta_x, thread_num);

        // Parallel computation with ForkJoinPool
        ex3(thread_num);
    }

    // Exercises
    // ---------------
    private static void ex1(double x_start, double x_end, double delta_x)
    {
        System.out.println("----- Sequential Integral Computation -----");

        CallableIntegral integral = new CallableIntegral(x_start, x_end, delta_x);
        double result = integral.compute_integral();

        System.out.printf("Final integral: %f\n", result);
    }

    private static void ex2(double x_start, double x_end, double delta_x, int thread_num)
    {
        System.out.println("----- Parallel Computation with Thread Pool -----");

        // Create thread pool
        ExecutorService executor = Executors.newFixedThreadPool(thread_num);

        // Distribute work
        double range = (x_end - x_start) / (thread_num * 2);
        List<Future<Double>> results = new ArrayList<>();

        for(int i = 0; i < thread_num * 2; i++)
        {
            double local_start = x_start + range * i;
            double local_end = local_start + range;
            CallableIntegral task = new CallableIntegral(local_start, local_end, delta_x);

            // Call the executor
            results.add(executor.submit(task));
        }

        // Consolidate results
        double total_integral = 0.0;
        try
        {
            for (Future<Double> result : results)
            {
                total_integral += result.get();
            }
        }
        catch (InterruptedException | ExecutionException exception)
        {
            System.out.println("Exception: " + exception);
        }

        // End pool's work
        executor.shutdown();

        // Print the result
        System.out.printf("Final integral %f\n", total_integral);
    }

    private static void ex3(int thread_num)
    {
        System.out.println("----- Parallel Merge Sort with Fork Join Pool -----");

        // Create a random array
        int arr_size = 10;
        int min_num = 0;
        int max_num = 100;

        int[] arr = ThreadLocalRandom.current()
                .ints(arr_size, min_num, max_num)
                .toArray();

        // Array size at which the calculation will be performed sequentially
        int threshold = 2;

        // Create Fork Join instance
        try (ForkJoinPool pool = new ForkJoinPool(thread_num))
        {
            // Task for the pool
            MergeSortTask task = new MergeSortTask(arr, threshold);

            // Start threads and retrieve the result
            int[] result = pool.invoke(task);

            // Result printing
            System.out.println("Input array: " + Arrays.toString(arr));
            System.out.println("Sorting result: " + Arrays.toString(result));
        }
    }

    private static void ex4(double x_start, double x_end, double delta_x, int thread_num)
    {
        System.out.println("----- Parallel integral computation with Runnable interface -----");

        // Create thread pool
        ExecutorService executor = Executors.newFixedThreadPool(thread_num);

        // Task list
        RunnableIntegral[] tasks = new RunnableIntegral[thread_num];

        // Work distribution
        double range_per_thread = (x_end - x_start) / thread_num;
        for (int i = 0; i < thread_num; i++)
        {
            // Prepare the task
            double local_start = x_start + range_per_thread * i;
            double local_end = local_start + range_per_thread;
            tasks[i] = new RunnableIntegral(local_start, local_end, delta_x);

            // Call the executor
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

        // Consolidate results
        double total_integral = 0.0;
        for (int i = 0; i < thread_num; i++)
        {
            total_integral += tasks[i].get_result();
        }

        // Print the result
        System.out.printf("Final integral: %f\n", total_integral);
    }
}

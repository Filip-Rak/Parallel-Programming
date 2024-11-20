import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class Main
{
    // Main Function
    // ---------------
    public static void main(String[] args)
    {
        // Common arguments
        double x_start = 0;
        double x_end = Math.PI;
        double delta_x = 0.1;
        int thread_num = 6;

        // Read user input
        Scanner input = new Scanner(System.in);

        // System.out.print("dx: ");
        // delta_x = input.nextDouble();

        // Sequential integral calculation
        ex1(x_start, x_end, delta_x);

        // Parallel computation with thread pool
        ex2(x_start, x_end, delta_x, thread_num);

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
        double range = (x_end - x_start) / thread_num;
        List<Future<Double>> results = new ArrayList<>();

        for(int i = 0; i < thread_num; i++)
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
        
    }
}

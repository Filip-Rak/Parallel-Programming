import java.util.Arrays;
import java.util.concurrent.RecursiveTask;

public class MergeSortTask extends RecursiveTask<int[]>
{
    // Attributes
    // ---------------
    private final int[] array;
    private final int threshold;

    // Constructor
    // ---------------
    MergeSortTask(int[] array, int threshold)
    {
        this.array = array;
        this.threshold = threshold;
    }

    protected int[] compute()
    {
        // Exit condition - sequential sorting for small arrays
        if (array.length <= threshold)
        {
            Arrays.sort(array);
            return array;
        }

        // Split array into two
        int mid = array.length / 2;
        int[] left_part = Arrays.copyOfRange(array, 0, mid);
        int[] right_part = Arrays.copyOfRange(array, mid, array.length);

        // Create separate tasks for each side
        MergeSortTask left_task = new MergeSortTask(left_part, threshold);
        MergeSortTask right_task = new MergeSortTask(right_part, threshold);

        left_task.fork();   // Run in parallel within a new thread
        int[] result_right = right_task.compute();  // Main thread will proceed to work on right side
        int[] result_left = left_task.join();  // Retrieve the result from child thread

        // Consolidate and return the result
        return merge_arr(result_right, result_left);
    }

    private int[] merge_arr(int[] arr_a, int[] arr_b)
    {
        int[] result = new int[arr_a.length + arr_b.length];
        int i = 0, j = 0, k = 0;

        // Compare and add bigger results first
        while (i < arr_a.length && j < arr_b.length)
        {
            if (arr_a[i] <= arr_b[j])
                result[k++] = arr_a[i++];
            else
                result[k++] = arr_b[j++];
        }

        // Add leftovers
        while (i < arr_a.length)
            result[k++] = arr_a[i++];

        while (j < arr_b.length)
            result[k++] = arr_b[j++];

        return result;
    }
}

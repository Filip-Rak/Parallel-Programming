import java.util.concurrent.RecursiveTask;

class DivideTask extends RecursiveTask<int[]>
{
    int[] arrayToDivide;

    public DivideTask(int[] arrayToDivide)
    {
        this.arrayToDivide = arrayToDivide;
    }

    protected int[] compute()
    {

        // .......

        // DivideTask task1 = new DivideTask(....);
        // DivideTask task2 = new DivideTask(....);

        // .......

        //Wait for results from both tasks
        // int[] tab1 = task1.join();
        // int[] tab2 = task2.join();

        // combine_arr(tab1, tab2, scal_tab);

        return new int[]{};
    }

    private void combine_arr(int[] arr1, int[] arr2, int[] combined_arr)
    {
        int i = 0, j = 0, k = 0;
        while (i < arr1.length && j < arr2.length)
        {
            if (arr1[i] < arr2[j])
                combined_arr[k] = arr1[i++];
            else
                combined_arr[k] = arr2[j++];

            k++;
        }
        if (i == arr1.length)
        {
            for (int a = j; a < arr2.length; a++)
                combined_arr[k++] = arr2[a];
        }
        else
        {
            for (int a = i; a < arr1.length; a++)
                combined_arr[k++] = arr1[a];
        }
    }

}

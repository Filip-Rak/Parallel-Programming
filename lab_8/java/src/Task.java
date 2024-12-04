public class Task implements Runnable
{
    /* Attributes */
    private final Barrier barrier;

    /* Constructor */
    public Task(Barrier barrier)
    {
        this.barrier = barrier;
    }

    /* Thread Execution Method */
    public void run()
    {
        try
        {
            // Enter the barrier
            System.out.printf("Thread %s: Entering barrier 1\n", Thread.currentThread().getName());
            barrier.await();

            // Enter the barrier
            System.out.printf("Thread %s: Entering barrier 2\n", Thread.currentThread().getName());
            barrier.await();

            // Enter the barrier
            System.out.printf("Thread %s: Entering barrier 3\n", Thread.currentThread().getName());
            barrier.await();
        }
        catch (InterruptedException ignore) {}


    }
}

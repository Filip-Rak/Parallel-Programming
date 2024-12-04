public class Barrier
{
    /* Attributes */
    private int total_threads;
    private int waiting_threads;

    /* Constructor */
    public Barrier(int thread_num)
    {
        this.total_threads = thread_num;
        this.waiting_threads = 0;
    }

    /* Public Methods */
    public synchronized void await() throws InterruptedException
    {
        waiting_threads += 1;
        if (waiting_threads < total_threads)
            wait();
        else
        {
            waiting_threads = 0;
            notifyAll();
            System.out.println("-------------------");
        }
    }
}

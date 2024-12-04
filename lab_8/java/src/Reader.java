import java.util.Random;

public class Reader implements Runnable
{
    /* Attributes */
    private final Library library;
    private final Random random = new Random();

    /* Constructor */
    public Reader(Library library)
    {
        this.library = library;
    }

    /* Public Methods */
    public void run()
    {
        try
        {
            while (true)
            {
                library.start_reading();

                // Wait before exiting
                int sleep_time = 500 + random.nextInt(1000);
                System.out.println("&&& Reader: " + Thread.currentThread().getName() + " will read for: " + sleep_time);
                Thread.sleep(sleep_time);

                library.end_reading();
                System.out.println("&&& Reader: " + Thread.currentThread().getName() + " exiting now");

                // Wait before coming back
                Thread.sleep(sleep_time * 2);
            }
        }
        catch (InterruptedException ignore) {}
    }
}

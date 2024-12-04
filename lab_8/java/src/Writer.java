import java.util.Random;

public class Writer implements Runnable
{
    /* Attributes */
    private final Library library;
    private final Random random = new Random();

    /* Constructor */
    public Writer(Library library)
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
                library.start_writing();

                // Wait before exiting
                int sleep_time = 1000 + random.nextInt(2000);
                System.out.println("### Writer: " + Thread.currentThread().getName() + " will write for: " + sleep_time);
                Thread.sleep(sleep_time);

                library.end_writing();
                System.out.println("### Writer: " + Thread.currentThread().getName() + " exiting now");

                // Wait a bit to prevent constant stealing of library
                Thread.sleep(sleep_time * 2);
            }
        }
        catch (InterruptedException ignore) {}
    }
}

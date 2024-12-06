import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Library
{
    /* Attributes */
    private final ReentrantLock  library_lock = new ReentrantLock();
    private final Condition readers = library_lock.newCondition();
    private final Condition writers = library_lock.newCondition();

    private int reader_count = 0;
    private int writer_count = 0;   // Instead of boolean for error checking

    private final int max_readers;

    /* Constructor */
    public Library(int max_readers)
    {
        this.max_readers = max_readers;
    };

    /* Public Methods */

    // Reader Methods
    public void start_reading() throws InterruptedException
    {
        library_lock.lock();

        try
        {
            // Wait until the library is clear of writers
            while (writer_count > 0)
            {
                // Waiting unlocks the library
                readers.await();
            }

            // Join the library
            reader_count += 1;
        }
        finally
        {
            check_for_errors();
            library_lock.unlock();
        }
    }

    public void end_reading()
    {
        library_lock.lock();

        try
        {
            // Decrement the number of readers
            reader_count -= 1;
            if (reader_count == 0 && library_lock.hasWaiters(writers))
            {
                // Signal writers if no readers or writers are within the library
                writers.signal();
            }
        }
        finally
        {
            check_for_errors();
            library_lock.unlock();
        }
    }

    // Writer methods
    public void start_writing() throws InterruptedException
    {
        library_lock.lock();

        try
        {
            // Make sure no writers or readers are present
            while (reader_count > 0 || writer_count > 0)
            {
                writers.await();
            }

            // Enter the library
            writer_count += 1;
        }
        finally
        {
            check_for_errors();
            library_lock.unlock();
        }
    }

    public void end_writing()
    {
        library_lock.lock();

        try
        {
            // Exit library
            writer_count -= 1;

            // Signal other threads
            if (library_lock.hasWaiters(readers))
                readers.signalAll();
            if(library_lock.hasWaiters(writers))
                writers.signal();
        }
        finally
        {
            check_for_errors();
            library_lock.unlock();
        }
    }

    /* Private Methods */
    private void check_for_errors()
    {
        String population = "----\nReaders: " + reader_count + "\nWriters: " + writer_count + "\n";
        System.out.printf(population);

        String error_string = population + "Error:\n";
        boolean failure = false;
        if (writer_count > 1)
        {
            error_string += "---- Number of writers is higher than one!\n";
            failure = true;
        }
        if (writer_count < 0)
        {
            error_string += "---- Number of writers is negative!\n";
            failure = true;
        }
        if (reader_count > max_readers)
        {
            error_string += "---- Number of readers is higher than max!\n";
            failure = true;
        }
        if (reader_count < 0)
        {
            error_string += "---- Number of readers is negative!\n";
            failure = true;
        }

        if(failure)
        {
            error_string += "---- Exiting...\n";
            System.err.println(error_string);

            System.exit(-1);
        }
    }
}

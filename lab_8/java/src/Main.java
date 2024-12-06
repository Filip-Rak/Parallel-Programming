public class Main
{
    /* Main Function */
    public static void main(String[] args)
    {
        ex1();
        // ex2();
    }

    /* Exercises */
    private static void ex1()
    {
        int reader_num = 10;
        int writer_num = 3;
        Library library = new Library(reader_num);

        // Create and run readers
        for (int i = 0; i < reader_num; i++)
            new Thread(new Reader(library), "Reader-" + i).start();

        // Create and run writers
        for (int i = 0; i < writer_num; i++)
            new Thread(new Writer(library), "Writer-" + i).start();
    }

    private static void ex2()
    {
        int thread_num = 5;
        Barrier barrier = new Barrier(thread_num);

        for (int i = 0; i < thread_num; i++)
        {
            new Thread(new Task(barrier), Integer.toString(i)).start();
        }

    }
}

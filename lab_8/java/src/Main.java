public class Main
{
    public static void main(String[] args)
    {
        int reader_num = 10;
        int writer_num = 3;
        Library library = new Library(reader_num, writer_num);

        // Create and run readers
        for (int i = 0; i < reader_num; i++)
            new Thread(new Reader(library), "Reader-" + i).start();

        // Create and run writers
        for (int i = 0; i < writer_num; i++)
            new Thread(new Writer(library), "Writer-" + i).start();
    }
}

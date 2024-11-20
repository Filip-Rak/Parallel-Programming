public class Main
{
    // Main Function
    // ---------------
    public static void main(String[] args)
    {
        // Sequential integral calculation
        ex1();
    }

    // Exercises
    // ---------------
    public static void ex1()
    {
        CallableIntegral integral = new CallableIntegral(0, Math.PI, 0.1);
        integral.compute_integral();
    }
}

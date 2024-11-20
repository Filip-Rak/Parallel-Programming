public class RunnableIntegral implements Runnable
{
    // Attributes
    // ----------------
    private final double x_start;
    private final double delta_x;
    private final int steps;

    private double result;

    // Constructor
    // ---------------
    RunnableIntegral(double x_start, double x_end, double dx)
    {
        double range = x_end - x_start;

        this.x_start = x_start;
        this.steps = (int) Math.ceil(range / dx);
        this.delta_x = range / steps;
        this.result = 0;
    }

    // Private Methods
    // ---------------
    private double get_function(double x)
    {
        return Math.sin(x);
    }

    // Public Methods
    // ---------------
    public void run()
    {
        double integral = 0;
        for(int i = 0; i< steps; i++)
        {
            double x1 = x_start + i * delta_x;
            double x2 = x1 + delta_x;

            double avg = get_function(x1) + get_function(x2);
            integral += avg / 2 * delta_x;
        }

        result = integral;
    }

    // Getters
    // ---------------
    public double get_result()
    {
        return result;
    }
}

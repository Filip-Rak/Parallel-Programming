import java.util.concurrent.Callable;

public class CallableIntegral implements Callable<Double>
{
    private final double delta_x;
    private final double x_start;
    private final int steps;

    public CallableIntegral(double x_start, double x_end, double delta_x)
    {
        double range = x_end - x_start;

        this.x_start = x_start;
        this.steps = (int)Math.ceil(range / delta_x);
        this.delta_x = range / steps;

        System.out.println("Creating an instance of CallableIntegral");
        System.out.println("x_start = " + x_start + ", x_end = " + x_end + ", N = " + steps);
        System.out.println("delta_x requested = " + delta_x + ", delta_x final = " + this.delta_x);

    }

    private double get_function(double x)
    {
        return Math.sin(x);
    }

    public double compute_integral()
    {
        double integral = 0;
        for(int i = 0; i< steps; i++)
        {
            double x1 = x_start + i * delta_x;
            double x2 = x1 + delta_x;
            double avg = get_function(x1) + get_function(x2);
            integral += avg / 2 * delta_x;
        }

        System.out.println("Partial integral: " + integral);
        return integral;
    }

    public Double call()
    {
        return compute_integral();
    }
}

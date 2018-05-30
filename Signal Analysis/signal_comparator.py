import matplotlib.pyplot as pyplot

class Signal(object):
    def load(self, time_values, amplitude_values):
        self.time_values = time_values
        self.amplitude_values = amplitude_values
        self.period = time_values[1] - time_values[0]
        self.frecuency = 1 / self.period
        self.max_amplitude = max(max(amplitude_values), abs(min(amplitude_values)))
        self.points = len(time_values)
    def __init__(self, data):
        self.load(data[0], data[1])


def import_signal(file_name):
    with open(file_name, 'r') as f:
        read_data = f.read().strip().split()
        time_values = [float(read_data[k]) for k in range(0, len(read_data), 2)]
        amplitude_values = [float(read_data[k]) for k in range(1, len(read_data), 2)]
        # print(time_values, amplitude_values)
    return time_values, amplitude_values


def faster_slower_signal(signal_1, signal_2):
    if signal_1.frecuency <= signal_2.frecuency:
        slower_signal, faster_signal = signal_1, signal_2
        frec_ratio = int(signal_2.frecuency / signal_1.frecuency)  # MUST BE FREC MULTIPLE
    else:
        slower_signal, faster_signal = signal_2, signal_1
        frec_ratio = int(signal_1.frecuency / signal_2.frecuency)  # MUST BE FREC MULTIPLE
    print("Max Frecuency:", faster_signal.frecuency, "Min Frecuency:", slower_signal.frecuency, "Frecuency Ratio:", frec_ratio)
    return faster_signal, slower_signal, frec_ratio


def retardo(faster_signal, slower_signal, frec_ratio):
    max_autocorrelation = float('-inf')
    delay = 0
    for offset in range(-faster_signal.points,faster_signal.points):
        autocorrelation_sum = 0
        offset_min_index = - offset //frec_ratio
        offset_max_index = (faster_signal.points - offset) // frec_ratio
        for index in range(max(offset_min_index, 0), min(offset_max_index, slower_signal.points)):
            autocorrelation_sum += slower_signal.amplitude_values[index] * faster_signal.amplitude_values[frec_ratio * index + offset]
        autocorrelation_sum = abs(autocorrelation_sum)   # ABS IS NECESARY TO CONSIDER INVERSION
        if autocorrelation_sum > max_autocorrelation:
            delay = offset
            max_autocorrelation = autocorrelation_sum
    print("The delay was of %d samples or %f seconds" % (delay, faster_signal.period * delay))
    return delay, faster_signal.period * delay


def amplitude(faster_signal, slower_signal, frec_ratio, sample_delay):
    cross_signal = 0
    square_signal = 0
    initial_error = 0
    delay_min_index = - sample_delay //frec_ratio
    delay_max_index = (faster_signal.points - sample_delay) // frec_ratio
    elements = len(range(max(delay_min_index, 0), min(delay_max_index, slower_signal.points)))
    for index in range(max(delay_min_index, 0), min(delay_max_index, slower_signal.points)):
        cross_signal += slower_signal.amplitude_values[index] * faster_signal.amplitude_values[frec_ratio * index + sample_delay]
        square_signal += slower_signal.amplitude_values[index] * slower_signal.amplitude_values[index]
        initial_error += (slower_signal.amplitude_values[index] - faster_signal.amplitude_values[frec_ratio * index + sample_delay]) ** 2
    amplitude = cross_signal / square_signal
    final_error = 0
    for index in range(max(delay_min_index, 0), min(delay_max_index, slower_signal.points)):
        final_error += (amplitude * slower_signal.amplitude_values[index] - faster_signal.amplitude_values[frec_ratio * index + sample_delay]) ** 2
    initial_error /= elements
    final_error /= elements
    print("Initial Amplitude Square Error:", initial_error, "Final Amplitude Square Error:", final_error)
    print("Slower Signal should be multiplied by %f " % amplitude)
    return amplitude


def graphic(faster_signal, slower_signal, frec_ratio, sample_delay, amplitude, points, title, xlabel, ylabel, faster_signal_name, slower_signal_name, filename):
    faster_decimated = [faster_signal.amplitude_values[index] for index in range(sample_delay, faster_signal.points, frec_ratio)]
    time_values = [slower_signal.time_values[index] for index in range(points)]
    slower_points = [slower_signal.amplitude_values[index] for index in range(points)]
    faster_points = [faster_decimated[index]/amplitude for index in range(points)]
    pyplot.figure()
    pyplot.title(title)
    pyplot.xlabel(xlabel)
    pyplot.ylabel(ylabel)
    pyplot.plot(time_values, faster_points, label = faster_signal_name, color = 'blue')
    pyplot.plot(time_values, slower_points, label = slower_signal_name, color = 'red')
    pyplot.legend()
    pyplot.savefig(filename)
    pyplot.show()


# IMPORT SIGNALS
signal_1 = Signal(import_signal("interpolation_input.dat"))
signal_2 = Signal(import_signal("interpolation_output.dat"))
# SEPARATE SIGNALS
faster_signal, slower_signal, frec_ratio = faster_slower_signal(signal_1, signal_2)
# DELAY
sample_delay, time_delay = retardo(faster_signal, slower_signal, frec_ratio)
# AMPLITUDE DELAY
amplitude_slower_signal = amplitude(faster_signal, slower_signal, frec_ratio, sample_delay)

# DELAY PLOT
graphic(faster_signal, slower_signal, frec_ratio, 0, amplitude_slower_signal, 100, "Signal Delay", "Time", "Amplitude", "Interpolated Signal", "Input Signal", "signal_delay_difference.png")
graphic(faster_signal, slower_signal, frec_ratio, sample_delay, 1, 100, "Signal Amplitude Difference", "Time", "Amplitude", "Interpolated Signal", "Input Signal", "signal_amplitude_difference.png")

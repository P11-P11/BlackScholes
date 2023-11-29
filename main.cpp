#define _USE_MATH_DEFINES // nos da constantes numericas

#include <iostream>
#include <cmath>
#include <vector>
#include <numeric> // para usar accumulate
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

/// nos ahorra usar std ::
using namespace std;

/// S -> stock price
/// T -> tiempo
/// K -> strike price
/// r -> risk free interest rate
/// sigma -> varianza

// Standard normal probability density function
double norm_pdf(const double& x) {
   /// recuerda formula de la densidad
    /*
     *  una normal cero uno distribuye como
     *  uno sobre raiz cuadrada de dos pi por e a la menos x² sobre dos
     *
     *  1/(sqrt(2pi) )*exp(-x²/2)
     * */
    return (1.0/(pow(2*M_PI,0.5)))*exp(-0.5*x*x);
}

/// Aproxima la acumulada de una normal 0 1
double norm_cdf(const double& x) {
    /// explicacion de donde sale la formula
    /*
     * esta es una formulacion un poco magica que
     * se le ocurrió a mark joshi
     * las aproximaciones son buenisimas
     * en el notebook de colab podemos ver algunos experimentos numericos
     * */

    if (abs(x) < 1e-5){
        return 0.5;
    }
    if (x < -6){ /// la probabilidad es casi 1 o 0 en cualquiera de esos casos
        return 0;
    }
    if (x > 6){
        return 1;
    }
    double k = 1.0/(1.0 + 0.2316419*x);
    double k_sum = k*(0.319381530 + k*(-0.356563782 + k*(1.781477937 + k*(-1.821255978 + 1.330274429*k))));

    if (x >= 0.0) {
        return (1.0 - (1.0/(pow(2*M_PI,0.5)))*exp(-0.5*x*x) * k_sum);
    } else {
        return 1.0 - norm_cdf(-x);
    }
}

/// Calcula d1
double calculate_d1(double S, double K, double T, double r, double sigma){
    return (log(S / K) + (r + 0.5 * pow(sigma, 2)) * T) / (sigma * sqrt(T));
}
/// Calcula el precio de una call option
double black_scholes_call(double S, double K, double T, double r, double sigma) {
    if (T < 1e-3) {
        /// es el valor intrinseco , se deduce de tomar T a cero en el modelo
        return max(S - K, 0.0);
    }

    double d1 = calculate_d1(S,K,T,r,sigma);
    double d2 = d1 - sigma * sqrt(T);

    return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2); /// retorna el call_option_price
}

double calculate_vega(double S, double K, double T, double r, double sigma) {
    double d1 = calculate_d1(S,K,T,r,sigma);
    double vega = S * sqrt(T) * norm_pdf(d1);
    return vega;
}

double calculate_implied_volatility_bisection(double option_price, double S, double T, double K = 1033.0, double r = 1.00, double lower_bound = 0.0, double upper_bound = 2.0, int max_iterations = 250, double tolerance = 1e-6) {


    double mid_point;

    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        mid_point = (lower_bound + upper_bound) / 2.0;

        // Calcula option price at midpoint
        double option_price_calculated = black_scholes_call(S, K, T, r, mid_point);

        // Updates
        if (option_price_calculated < option_price) {
            lower_bound = mid_point;
        } else {
            upper_bound = mid_point;
        }

        // Check for convergence
        if (abs(option_price_calculated - option_price) < tolerance)  {
            break;
        }
    }

    return mid_point;
}

double hybridMethod(double option_price, double S, double T, double K = 1033.0, double r = 1.00,int max_iterations = 250, double tolerance = 1e-6){
     /// idea del metodo y algunos comentarios de newton en general
    /*
  * usar como initial guess de newton el valor al que convergió bisection
     el metodo es muy sensible al dato inicial y puede llegar a ser inestable dependiendo del mismo
     ademas divide por vega por lo que si este se va a cero o aun peor, es cero,
     el metodo tendra dificultades en converger

  * */
    double volatility = calculate_implied_volatility_bisection(option_price,S,T); /// usamos un valor random como inicial
    for (int iteration = 0; iteration < max_iterations; ++iteration){
    /// Calcula option price
        double option_price_calculated = black_scholes_call(S, K, T, r, volatility);
    /// Calcula vega
        double vega = calculate_vega(S, K, T, r, volatility);
        if (vega == 0){
            return -1.0; /// señalamos -1 como error numerico insalvable
        }
    /// Calcula price difference
        double price_difference = option_price_calculated - option_price;
    /// Update volatility
        double volatility_difference = abs(option_price_calculated - option_price) / vega;

        if (price_difference < tolerance && volatility_difference < 1e-3) {
            break;
        }

        volatility -= price_difference / vega;
    }

    return volatility;
}

/// este metodo dependiendo del initial guess que es random puede llegar a generar segmentation faults
/// por lo que decidi no usarlo, de todos modos en una corrida que no tuvo problemas de memoria genero los datos "correctamente"
/// como me los quiero llevar a python para comparar los metodos lo dejo aca para que vean de donde salió

double calculate_implied_volatility_newton(double option_price, double S, double T, double K = 1033.0, double r = 1.00,int max_iterations = 250, double tolerance = 1e-6){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(0.60, 0.80);

/// Explica como generar random numbers
/*
gracias stack exchange

"std::random_device is used to seed the random number generator. The std::mt19937 is a commonly used engine for pseudo-random number generation.
std::uniform_real_distribution<double> defines a distribution. In this case, it's a uniform distribution for double values between 0 and 1.
distribution(gen) generates a random value using the specified distribution and generator."
 */
    double volatility = distribution(gen); /// usamos un valor random como inicial

    if (T < 0.003) {
/// hardcodea el retorno para evitar inestabilidad numerica para valores cercanos a cero
        return 0.0;
    }

    for (int iteration = 0; iteration < max_iterations; ++iteration){
// Calcula option price
        double option_price_calculated = black_scholes_call(S, K, T, r, volatility);
// Calcula vega
        double vega = calculate_vega(S, K, T, r, volatility);
        if (vega == 0){
            return -1.0; /// señalamos -1 como error numerico insalvable
        }
// Calcula price difference
        double price_difference = option_price_calculated - option_price;
// Update volatility
        double volatility_difference = abs(option_price_calculated - option_price) / vega;

        if (price_difference < tolerance && volatility_difference < 1e-3) {
            break;
        }

        volatility -= price_difference / vega;
    }

    return volatility;
}


vector<vector<double>> leerCSV(const string &archivo) {
    vector<vector<double>> datos;

    ifstream file(archivo);

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo: " << archivo << endl;
        return datos;
    }

    string linea;
    bool esPrimeraLinea = true;  // Flag to check if it's the first line (header)

    while (getline(file, linea)) {
        vector<double> fila;
        istringstream ss(linea);
        string campo;

        if (esPrimeraLinea) {
            // Skip the first line (header)
            esPrimeraLinea = false;
            continue;
        }
        while (getline(ss, campo, ',')) {

            try {
                // Intenta convertir el campo a double y agregarlo a la fila
                fila.push_back(stod(campo));
            } catch (const invalid_argument &ia) {
                
                cerr << "Error al convertir a double: " << ia.what() << endl;
            }
        }

        datos.push_back(fila);
    }

    file.close();

    return datos;
}

/// calcula la volatilidad realizada y arma el vector de retorno
/// recibe directamente los datos leidos del csv
vector<double> make_realized_volatility_series(const vector<vector<double>> &data) {
    vector<double> realized_volatility_series;

    size_t n = data.size();
    /// chekeo tener los datos suficientes
    if (n < 21) {
        cerr << "Error: No hay suficientes datos para calcular la volatilidad." << endl;
        return realized_volatility_series;
    }

    // Calcula la volatilidad realizada
    for (size_t i = 0; i < 20;++i){
        /// para evitar tener vectores de tamaños diferentes ponemos nans
        double nanValue = std::numeric_limits<double>::quiet_NaN();
        realized_volatility_series.push_back(nanValue);
    }
    for (size_t i = 20; i < n; ++i) {
        double sum_log_returns = 0.0;
        /// j = 1 j = 2 j = 3
        /// optionPrice stockPrice time_to_T
        // Calcula la suma de los retornos logarítmicos en la ventana de 21 días
        for (size_t j = i - 19; j <= i; ++j) {
            sum_log_returns += log(data[j][2] / data[j - 1][2]);
        }

        // Calcula la desviación estándar y multiplica por la raíz cuadrada del número de períodos en un año
        double volatility = sqrt(252.0) * sqrt(sum_log_returns * sum_log_returns / 21.0);

        realized_volatility_series.push_back(volatility);
    }

    return realized_volatility_series;
}

/// la idea es armar dos vectores de volatilidades para poder comparar metodos y sacar mejores conclusiones
vector<vector<double>> make_implied_volatility_series(const vector<vector<double>> &data){

    vector<double> implied_volatility_series_bisection;
    vector<double> implied_volatility_series_hibrido;

    // recordatorio de los indices
    /// j = 1 j = 2 j = 3
    /// optionPrice stockPrice time_to_T
    size_t n = data.size();
    /// arma los vectores de retorno
    for (size_t i = 0; i < n; ++i) {
            double implied_volatility_bisection = calculate_implied_volatility_bisection(data[i][1],data[i][2],data[i][3]);

            implied_volatility_series_bisection.push_back(implied_volatility_bisection);

            double implied_volatility_hibrido = hybridMethod(data[i][1], data[i][2], data[i][3]);

            implied_volatility_series_hibrido.push_back(implied_volatility_hibrido);

    }
    return {implied_volatility_series_bisection,implied_volatility_series_hibrido};
}

void escribirCSV(const string &nombre_archivo, vector<vector<double>> &implied_series, const vector<double> &realized_series) {
    ofstream archivo(nombre_archivo);

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombre_archivo << " para escritura." << endl;
        return;
    }

    // Escribir encabezados
    archivo << "hibrid,bisection,realized" << endl;

    // Asegurarse de que ambos vectores tengan la misma longitud
    size_t n = implied_series[0].size();
    if (n != realized_series.size()) {
        cerr << "Error: Vectores de diferente longitud." << endl;
        return;
    }

    // Escribir datos
    for (size_t i = 0; i < n; ++i) {
        archivo << implied_series[0][i] << "," << implied_series[1][i] << "," << realized_series[i] << endl;
    }

    archivo.close();
}


int main() {
    vector<vector<double>> data = leerCSV("/home/clinux01/Descargas/preciosTiempos.csv");

    vector<double> realized_series =  make_realized_volatility_series(data);

    vector<vector<double>> implied_series =  make_implied_volatility_series(data);


    string rutaNombre = "/home/clinux01/Documentos/discoveriesHibrido.csv";

    escribirCSV(rutaNombre,implied_series,realized_series);

    return 0;
}



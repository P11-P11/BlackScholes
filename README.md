BlackScholes
Estimación de Volatilidad Black-Scholes

Este repositorio contiene el código fuente en C++ para la estimación de la volatilidad implícita utilizando dos métodos: bisección y un método híbrido. Además, se calcula la volatilidad realizada basada en datos históricos de precios de acciones.

La estructura de trabajo que seguí consistió en 3 partes:

1ra) Estudiar

   Para tener una familiaridad mayor con las variables, conceptos y modelos financieros en los que trabajé, utilicé el libro "John C Hull Options, Futures and Other Derivatives".

2da) Armar modelos en Python

   En este momento del desarrollo llevé todo lo estudiado a la práctica e implementé los métodos numéricos que posteriormente llevé a C++. Este paso fue fundamental para generar intuición respecto a la estabilidad numérica de los métodos en este problema en particular.

3ra) Llevar a C++ lo hecho en Python

   Este fue probablemente el paso más directo de los 3, ya que simplemente fue hacer la traducción de un lenguaje a otro con los cuidados que C++ requiere.

Un comentario del CSV que genera C++:

   El archivo generado contendrá únicamente los resultados obtenidos mediante el método híbrido (bisección + Newton) y el método de bisección. Opté por excluir el método de Newton estándar debido a diversos problemas de estabilidad, incluyendo fallos de segmentación, que surgieron a partir de la aleatoriedad del valor inicial asignado. La curva que se visualiza en el gráfico corresponde a una de las iteraciones exitosas.

Por último, recomiendo fuertemente visitar el notebook que armé para la ocasión. Allí entro en mucho más detalle y puede verse cómo trabajé.

https://colab.research.google.com/drive/1sCMAdY0nYxQd-Sr5IQL1LpYhN4oQXlsz#scrollTo=lBecnGbLBli1

De todos modos, lo agrego como IPYNB por si es más cómodo para ustedes.

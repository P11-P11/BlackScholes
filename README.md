# BlackScholes

Estimación de Volatilidad Black-Scholes
Este repositorio contiene el código fuente en C++ para la estimación de la volatilidad implícita utilizando dos métodos: bisección y un método híbrido. Además, se calcula la volatilidad realizada basada en datos históricos de precios de acciones.

La estructura de trabajo que seguí consistió en 3 partes:

1ra) Estudiar
   
   para tener una familiaridad mayor con las variables, conceptos y modelos financieros en los que trabajé use el libro
  "John C Hull Options,Futures and Other Derivatives"

2da) Armar modelos en python
    En este momento del desarollo lleve todo lo estudiado a la practica e implemente los metodos numericos que a posteriori
    lleve a c++, este paso fue fundamental para generar intuicion respecto a la estabilidad numerica de los metodos en 
    este problema en particular
    
3ra) llevar a c++ lo hecho en python
  
  Este fue probablemente el paso mas directo de los 3 ya que fue simplemente hacer la traduccion de un lenguaje a otro con los cuidados que c++ requiere.

Un comentario del csv que genera C++

El archivo generado contendrá únicamente los resultados obtenidos mediante el método híbrido (bisección + Newton) y el método de bisección.
Opté por excluir el método de Newton estándar debido a diversos problemas de estabilidad, incluyendo fallos de segmentación. Que surgieron a partir de la aleatoriedad del valor inicial asignado.
La curva que se visualiza en el gráfico corresponde a una de las iteraciones exitosas.

Por ultimo Recomiendo fuertmente visitar el notebook que arme para la ocasion
alli entro en mucho mas detalle y puede verse como trabajé

https://colab.research.google.com/drive/1sCMAdY0nYxQd-Sr5IQL1LpYhN4oQXlsz#scrollTo=lBecnGbLBli1

De todos modos lo agrego como ipynb por si es mas comdo para ustedes

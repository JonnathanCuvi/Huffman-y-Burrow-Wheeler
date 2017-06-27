/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package bwinterfas;

public class RLE {
 
    private String compresion;
    private final int MAXIMO_VALORES;
 
    public RLE(String x) {
        MAXIMO_VALORES = 10;
        compresion = comprimir(x);
 
    }
 
    private String comprimir(String x) {
        String aux = "";
        int contador = 1;
        char caracter = 0;
 
        for (int i = 0; i < x.length()-1; i++) {
            caracter = x.charAt(i);
            //System.out.println(caracter + " --- " + x.charAt(i+1)); 
 
            if (caracter == x.charAt(i+1)){
            	//System.out.println(contador); 
            	contador ++;
            }else{
            	aux += caracter+""+ contador;
            	caracter =  x.charAt(i+1);
            	contador = 1;
            }
           // System.out.println(aux); 
        }
        aux += caracter+""+ contador;
        return aux;
    }
 
    public String toString(){
        return compresion;
    }
}
 
 
 

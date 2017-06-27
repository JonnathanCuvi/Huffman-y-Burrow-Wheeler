package bwinterfas;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;


/**
 *
 * @author BeLeN
 */
public class Lectura_Archivo{
    File archivo = null;
    FileReader fr = null;
    BufferedReader br = null;
    String texto = "";

    public String leerArchivo(String rutaArchivo){
        //Abirmos el archivo
        try {
            archivo = new File(rutaArchivo);
            FileReader fr = new FileReader(archivo);
            br = new BufferedReader(fr);

            // Lectura del fichero
            String linea;
           
            while ((linea = br.readLine()) != null) {
                //System.out.println("linea " + linea);
                texto = texto.concat(linea);
                texto = texto.concat(" ") ;
                //System.out.println("texto " + texto);
            }
        } catch (Exception e) {
            System.out.println("Excepcion en leer " + archivo + ": " + e);
        } finally {
            // En el finally cerramos el fichero, para asegurarnos
            // que se cierra tanto si todo va bien como si salta 
            // una excepcion.
            try {
                if (null != fr) {
                    fr.close();
                }
            } catch (Exception e2) {
                System.out.println("Excepcion leyendo fichero " + archivo + ": " + e2);
            }
        }
        return texto;
    }
}

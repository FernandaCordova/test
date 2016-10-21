#define EXT_PID   255
int16  longitud1587 = 0;


int get_packet_j1587(char* packet_ptr, int* descripcion)
{
    int   answer;
    int16 saveIndex = 0;
    int   MID = 0;
    int16 PID = 0;
    int   M256PID = 0;
    int   checksum = 0;
    int   limitCount;       //limite de 21 bytes por mensaje
    int   length_PID;
    int   top; 
    
    saveIndex  = next_out;  //guardamos el index del ultimo byte que se obtuvo del buffer
    answer     = TRUE;  
    limitCount = 0;        //Iniciamos el contador en 0
    
    packet_ptr[limitCount] = bgetc();        // get MID
    descripcion[limitCount]= 'M';
    MID = packet_ptr[limitCount];
    
    if((MID == 128) || (MID == 130) || (MID == 136) || (MID == 144) || (MID == 132)) /// MID con los que cuneta VOLVO  (MID == 140) (MID == 150)
    {
        
        limitCount++;
        packet_ptr[limitCount] = bgetc();              // get PID
        
        GetPID:
        
        PID = packet_ptr[limitCount]; //Get first PID
        descripcion[limitCount]= 'P';
        
        if(PID == EXT_PID) /*extension PID*/
        {
            descripcion [limitCount]='E';
            limitCount++;
            packet_ptr[limitCount] = bgetc();
            descripcion[limitCount]= 'P';
            M256PID = packet_ptr[limitCount];
            
            PID = 256 + M256PID;   /*256-511 PID definitions*/
            
            if(PID > 255 && PID < 512)
            {
            }
            else
            {
                answer = FALSE;
            }
        }
        
        
        if ((PID < 128)|| ( (PID>255) && (PID < 384)))
        {
            length_PID = 1;// one byte long PIDs 0-127 Y 256-383
        }
        
        if (((PID > 127) && (PID < 192)) || ((PID > 383) && (PID < 448)))
        {
            length_PID = 2; // tow byte long PIDs 128-191 Y 384-447
        }
        
        if (((PID > 191) && (PID < 254)) || ((PID > 447) && (PID < 510)))
        {
            limitCount++;
            packet_ptr[limitCount] = bgetc();
            descripcion [limitCount]='L';
            length_PID = packet_ptr[limitCount];// the first byte  following these PIDs contain the number of data parameter byte, PIDs 192-253 y 448-509
        }        
        
        if((PID == 254)||(PID == 510 )||(PID == 193)||(PID == 194 )) // data link escape PID
        {
            answer = FALSE;
        }
        
        top = limitCount + length_PID;// Cantidad de datos a tomar del buffer
        
        for(int i = limitCount ;i < top ; i++)
        {
            limitCount++;
            if(limitCount > 20)
            {
                return FALSE;
            }
            else
            {
                packet_ptr[limitCount] = bgetc();
                descripcion [limitCount]='D';
            }
        }
        
        /*if(limitCount > 20)// si la cuenta es mayor a 20 significa que nuestro paquete no es valido Max 21 bytes
        {
            answer = FALSE;
        }*/
        
        limitCount++;
        packet_ptr[limitCount] = bgetc();// aqui se obtine el posible checksum o PID;
        descripcion [limitCount]='C';

        if(answer == TRUE)// si se han recibido bytes validos se calcula el checksum
        {
            checksum = 0;
            
            for(int k = 0; k < limitcount +1; k++ )
            {
                checksum = (checksum + packet_ptr[k]);
            }
            
            if(checksum == 0) // si esto se cumple quiere decir que tenemos una trama valida
            {
                
            }
            else  // si no obtuvimos un chesum valido es posible que el byate actual no sea un cheksum, por lo tanto podria ser un PID asi que lo procesaremos de nuevo como PID
            {
                goto GetPID; 
            }
       }  
    }
    else
    {
        answer  = 0;
    }
    
    if (answer == FALSE)                        //Si hubo un error en los datos procesados se recupera el index_out para no perder ninguno de los bytes analizados.
    {
        saveIndex++;
        
        if(saveIndex == buffersize)       
        {
            saveindex = 0;
        }
        
        next_out = saveindex;                  //Con esto tomamos el siguiente byte como MID 
    }    
    
    if(answer == TRUE)
    {
        longitud1587 = limitCount; // escribimos el tamaño delpaquete en una variable global
    }
    else
    {
        longitud1587 = 0;
    }
    
    return answer;            
}

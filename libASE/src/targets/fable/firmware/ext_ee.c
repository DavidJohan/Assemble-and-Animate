#include <stdio.h>
#include <avr/io.h>
#include <ase/targets/fable/firmware/ext_ee.h>
#include <ase/targets/fable/firmware/i2c.h>

uint8_t ee_write_byte(uint8_t addr_highbyte,uint8_t addr_lowbyte, uint8_t data){
	int status=0;
	int debug=0;
	do{
		debug+=1;
		i2c_send_start();
		status=i2c_send_addr(EE_ADDR,TW_WRITE);
	}while(status==TW_MT_SLA_NACK && debug < MAX_RETRIES);
	if(debug>=MAX_RETRIES){
#ifdef DEBUG
		printf("EE: ERROR WRITING\n");
#endif
		return 1;
	}
	i2c_send_data(addr_highbyte);//addr_high byte
	i2c_send_data(addr_lowbyte);//addre_low_byte
	i2c_send_data(data);//data
	i2c_send_stop();
#ifdef DEBUG
	if(debug>1)
		printf("EE: Had to wait %d before write\n",debug);
#endif
	return 0;
}
uint8_t ee_read_byte(uint8_t addr_highbyte,uint8_t addr_lowbyte){
	int data=0;
	int debug=0;
	do{
		debug+=1; //debug:to see how many times looped in here
		i2c_send_start();
		data=i2c_send_addr(EE_ADDR,TW_WRITE);
	}while(data==TW_MT_SLA_NACK && debug < MAX_RETRIES);
	if(debug>=MAX_RETRIES){
#ifdef DEBUG
		printf("EE: ERROR READING\n");
#endif
		return 0xFF;
	}
	i2c_send_data(addr_highbyte);
	i2c_send_data(addr_lowbyte);
	i2c_send_start();
	i2c_send_addr(EE_ADDR,TW_READ);
	data=i2c_receive_data(0);
	i2c_send_stop();
#ifdef DEBUG
	if(debug>1)
		printf("EE: Had to wait %d before read\n",debug);
#endif
	return data;
}


uint8_t ee_seq_read(uint8_t howmanybytes,uint8_t* thatmanybytes,uint8_t addr_highbyte,uint8_t addr_lowbyte){
	int i=0;
	int debug=0;
	do{
		debug+=1; //debug:to see how many times looped in here
		i2c_send_start();
		thatmanybytes[0]=i2c_send_addr(EE_ADDR,TW_WRITE);
	}while(thatmanybytes[0]==TW_MT_SLA_NACK && debug < MAX_RETRIES);//ensure device is not busy on internal write cycle
	if(debug>=MAX_RETRIES){
#ifdef DEBUG
		printf("EE: ERROR SEQ READ\n");
#endif
		return 1;
	}
	i2c_send_data(addr_highbyte);
	i2c_send_data(addr_lowbyte);
	i2c_send_start();
	i2c_send_addr(EE_ADDR,TW_READ);
	for(i=0;i<howmanybytes;i++)
		thatmanybytes[i]=i2c_receive_data(i<(howmanybytes-1)?1:0);
	i2c_send_stop();
#ifdef DEBUG
	if(debug>1)
		printf("EE: Had to wait %d before read\n",debug);
#endif
	return 0;
}

uint8_t ee_seq_write(uint8_t howmanybytes,uint8_t* thatmanybytes,uint8_t addr_highbyte,uint8_t addr_lowbyte){
	if(howmanybytes > 128)
		printf("Warning: attempting to write more than EE page size!\n");
	int status=0;
	int i=0;
	int debug=0;
	do{
		debug+=1;
		i2c_send_start();
		status=i2c_send_addr(EE_ADDR,TW_WRITE);
	}while(status==TW_MT_SLA_NACK && debug < MAX_RETRIES);
	if(debug>=MAX_RETRIES){
#ifdef DEBUG
		printf("EE: ERROR SEQ WRITE\n");
#endif
		return 1;
	}
	i2c_send_data(addr_highbyte);//addr_high byte
	i2c_send_data(addr_lowbyte);//addre_low_byte
	for(i=0;i<howmanybytes;i++)
		i2c_send_data(thatmanybytes[i]);//data
	i2c_send_stop();
#ifdef DEBUG
	if(debug>1)
		printf("EE: Had to wait %d before write\n",debug);
#endif

	return 0;
}

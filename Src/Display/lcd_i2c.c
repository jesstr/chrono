#include <stdint.h>
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx.h"

/**
  * @brief I2C Periferal Interface
  */
#define I2C                                     I2C1

/**
  * @brief Master Transfer Request Direction
  */
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01


#define I2C_SEND_TIMEOUT_TXIS_MS       5

uint32_t Timeout = 0; /* Variable used for Timeout management */


void I2C_Send(uint8_t addr, uint8_t *pTransmitBuffer, uint8_t ubNbDataToTransmit)
{
    /* (1) Initiate a Start condition to the Slave device ***********************/

    /* Master Generate Start condition for a write request :              */
    /*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
    /*    - with a auto stop condition generation when transmit all bytes */
    LL_I2C_HandleTransfer(I2C, addr, LL_I2C_ADDRSLAVE_7BIT, ubNbDataToTransmit, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    /* (2) Loop until end of transfer received (STOP flag raised) ***************/
    Timeout = I2C_SEND_TIMEOUT_TXIS_MS;

    /* Loop until STOP flag is raised  */
    while (!LL_I2C_IsActiveFlag_STOP(I2C))
    {
        /* (2.1) Transmit data (TXIS flag raised) *********************************/

        /* Check TXIS flag value in ISR register */
        if (LL_I2C_IsActiveFlag_TXIS(I2C))
        {
            /* Write data in Transmit Data register.
            TXIS flag is cleared by writing data in TXDR register */
            LL_I2C_TransmitData8(I2C, (*pTransmitBuffer++));

            Timeout = I2C_SEND_TIMEOUT_TXIS_MS;
        }

        /* Check Systick counter flag to decrement the time-out value */
        if (LL_SYSTICK_IsActiveCounterFlag())
        {
            if (Timeout-- == 0)
            {
                ;
            }
        }
    }

    /* (3) Clear pending flags, Data consistency are checking into Slave process */

    /* End of I2C_SlaveReceiver_MasterTransmitter Process */
    LL_I2C_ClearFlag_STOP(I2C);
}

#if 0
void I2C_Send(uint8_t addr, uint8_t *pTransmitBuffer, uint8_t ubNbDataToTransmit)
{
    /* (1) Prepare acknowledge for Master data reception ************************/
    LL_I2C_AcknowledgeNextData(I2C, LL_I2C_ACK);
    
    /* (2) Initiate a Start condition to the Slave device ***********************/
    /* Master Generate Start condition */
    LL_I2C_GenerateStartCondition(I2C);

    /* (3) Loop until Start Bit transmitted (SB flag raised) ********************/
    /* Loop until SB flag is raised  */
    // while(!LL_I2C_IsActiveFlag_SB(I2C)) {
    while(!LL_I2C_IsActiveFlag_TXE(I2C)) {
        ;
    }

    /* (4) Send Slave address with a 7-Bit SLAVE_OWN_ADDRESS for a write request */
    LL_I2C_TransmitData8(I2C, addr | I2C_REQUEST_WRITE);

    /* (5) Loop until Address Acknowledgement received (ADDR flag raised) *******/
    /* Loop until ADDR flag is raised  */
    while(!LL_I2C_IsActiveFlag_ADDR(I2C)) {
        ;
    }

    /* (6) Clear ADDR flag and loop until end of transfer (ubNbDataToTransmit == 0) */
    /* Clear ADDR flag value in ISR register */
    LL_I2C_ClearFlag_ADDR(I2C);

    /* Loop until TXE flag is raised  */
    while(ubNbDataToTransmit > 0)
    {
        /* (6.1) Transmit data (TXE flag raised) **********************************/

        /* Wait TXE flag value in ISR register */
        while (!LL_I2C_IsActiveFlag_TXE(I2C)) {
            ;
        }

        /* Write data in Transmit Data register.
        TXE flag is cleared by writing data in TXDR register */
        LL_I2C_TransmitData8(I2C, (*pTransmitBuffer++));

        /* Wait BTF flag value in ISR register */
        // while (!LL_I2C_IsActiveFlag_BTF(I2C)) {
        while (!LL_I2C_IsActiveFlag_TXIS(I2C)) {
            ;
        }

        ubNbDataToTransmit--;
    }

    /* (7) End of tranfer, Data consistency are checking into Slave process *****/
    /* Generate Stop condition */
    LL_I2C_GenerateStopCondition(I2C);
}
#endif
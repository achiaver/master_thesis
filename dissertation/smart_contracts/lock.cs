







// 1 função a ser implementada... para liberar os tokens depois de um período
// https://docs.neo.org/en-us/sc/tutorial/Lock.html

public class Lock : SmartContract
{
    /**
    * The contract implements a function that specifies a certain timestamp.
    * Before the specified time stated, no one is allowed to withdraw any assets from the contract.
    * Once the time stated is reached, the contract owners can then withdraw the assets.
    * 
    * INPUTS
    *   timestamp = the lock time in the sample code, which is a Unix timestamp.
    *               You can calculate it yourself or use it: https://unixtime.51240.com/
    *   pubkey = insert the previous copy of the public key byte array
    *   signature = the private key?
    **/
    public static bool Main(uint timestamp, byte[] pubkey, byte[] signature)
    {
        Header header = Blockchain.GetHeader(Blockchain.GetHeight());
        if (header.Timestamp < timestamp)
            return false;
        return VerifySignature(signature，pubkey);
    }
}
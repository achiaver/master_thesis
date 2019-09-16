using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;

namespace Neo.SmartContract
{
    public class Contract1 : Framework.SmartContract
    {
        public static void Main()
        {
            byte[] scriptHash = "AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y".ToScriptHash();
            Account acc = Blockchain.GetAccount(scriptHash);
        }
    }
}
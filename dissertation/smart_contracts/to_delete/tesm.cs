/***************************************
 * Allow the transaction between peers *
****************************************/
using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;
using Neo.SmartContract.Framework.Services.System;
using System;
using System.ComponentModel;
using System.Numerics;
using Group.Assets;
// Define Group to better integrate the smart contracts
// tName = "Shareable Electricity Trading"
// tSymbol = "SEB" // SEB = Sistema Elétrico Brasileiro ;)
// groupPBkkey = "Ae2d6qj91YL3LVUMkza7WQsaTYjzjHm4z1"

namespace NEP5
{
    public class Trade : SmartContract
    {
        /****************
         * Basic Events *
         ****************/
        [DisplayName("transfer")]
        public static event Action<byte[], byte[], BigInteger> Transferred;

        
        /*
        Inputs:
                method - a Token feature to choose ('BalanceOf', 'decimals', 'name', 'symbol', 'supportedStandards', 'totalSupply', 'transfer')
                args - a list with the arguments required to complete some methods, such as the:
                            'BalanceOf' with the public address about the one who would like to check the number of tokens
                            'Transfer' with the peers public adresses who will make the exchange and the value in the transaction (what about the cost? how to settle this?)
         */
        public static object Main(string method, object[] args)
        {
            if (Runtime.Trigger == TriggerType.Verification)
            {
                // At the time of the smart contract execution...
                // https://docs.neo.org/en-us/sc/reference/api/neo.html
                return Runtime.CheckWitness(Owner);
            }
            else if (Runtime.Trigger == TriggerType.Application)
            {
                // https://docs.neo.org/en-us/sc/reference/api/System.html
                // Calling OR Get
                var callscript = ExecutionEngine.CallingScriptHash;

                if (method == "balanceOf") return BalanceOf((byte[])args[0]);

                if (method == "decimals") return Decimals();

                if (method == "name") return Name();

                if (method == "symbol") return Symbol();

                if (method == "supportedStandards") return SupportedStandards();

                if (method == "totalSupply") return TotalSupply();

                if (method == "transfer") return Transfer((byte[])args[0], (byte[])args[1], (BigInteger)args[2], callscript);
            }
            return false;
        }

        
        private static readonly byte[] Owner = groupPBkey.ToScriptHash(); // Owner Address
        
        [DisplayName("balanceOf")]
        public static BigInteger BalanceOf(byte[] account)
        {
            if (account.Length != 20)
                throw new InvalidOperationException("The parameter account SHOULD be 20-byte addresses.");
            StorageMap asset = Storage.CurrentContext.CreateMap(nameof(asset));
            return asset.Get(account).AsBigInteger();
        }
        [DisplayName("decimals")]
        public static byte Decimals() => 8; // WTF?

        private static bool IsPayable(byte[] to)
        // The cost of the transaction is payed by who is earning the tokens?
        // Why it is not declared as a method, nor inside a 'transfer'?
        {
            var c = Blockchain.GetContract(to);
            return c == null || c.IsPayable;
        }

        [DisplayName("name")]
        public static string Name() => tName; // name of the token

        [DisplayName("symbol")]
        public static string Symbol() => tSymbol; // symbol of the token

        [DisplayName("supportedStandards")]
        public static string[] SupportedStandards() => new string[] { "NEP-5", "NEP-7", "NEP-10" };

        [DisplayName("totalSupply")]
        public static BigInteger TotalSupply()
        {
            StorageMap contract = Storage.CurrentContext.CreateMap(nameof(contract));
            return contract.Get("totalSupply").AsBigInteger();
        }

#if DEBUG
        [DisplayName("transfer")] // Only for ABI file
        public static bool Transfer(byte[] from, byte[] to, BigInteger amount) => true;
#endif
        
        // Methods of actual execution
        private static bool Transfer(byte[] from, byte[] to, BigInteger amount, byte[] callscript)
        {
            // Check parameters
            if (from.Length != 20 || to.Length != 20)
                throw new InvalidOperationException("The parameters from and to SHOULD be 20-byte addresses.");
            
            if (amount <= 0)
                throw new InvalidOperationException("The parameter amount MUST be greater than 0.");
            
            if (!IsPayable(to))
                return false;
            
            if (!Runtime.CheckWitness(from) && from.AsBigInteger() != callscript.AsBigInteger())
                return false;
            
            StorageMap asset = Storage.CurrentContext.CreateMap(nameof(asset));
            var fromAmount = asset.Get(from).AsBigInteger();
            if (fromAmount < amount)
                // 'from' has less tokens than required for the transaction
                return false;
            
            if (from == to) // Can I transfer to myself?
                return true;

            // Reduce payer balances
            if (fromAmount == amount)
                asset.Delete(from); // replace with 0.? the wallet of 'from'
            else
                // fromAmount > amount
                asset.Put(from, fromAmount - amount);

            //Increase the payee balance
            var toAmount = asset.Get(to).AsBigInteger();
            asset.Put(to, toAmount + amount);

            Transferred(from, to, amount);
            return true;
        }
    }
}


// --------------------------------------------------
        // function that is always called when someone wants to transfer tokens.
        // --> atualizar o 'to' para array e incluir um loop na transferência dos tokens!
        private static bool Transfer(byte[] from, byte[] to, BigInteger value)
        {
            // No pay, no gain
            if (value <= 0) return false;
            
            // Check if who has requested the transfer is the same address who has created the tokens
            if (!Runtime.CheckWitness(from)) return false;
            
            // Why this check if it has already been verified?
            if (to.Length != 20) return false;
            
            BigInteger from_value = Storage.Get(Storage.CurrentContext, from).AsBigInteger();
            if (from_value < value) return false;
            if (from == to) return true; // dessa forma a função encerra-se sem fazer nada...é isso msm?
            
            if (from_value == value)
                Storage.Delete(Storage.CurrentContext, from);
            else
                Storage.Put(Storage.CurrentContext, from, from_value - value);
            
            // save the information of the values transferred on the SC memory space
            BigInteger to_value = Storage.Get(Storage.CurrentContext, to).AsBigInteger();
            Storage.Put(Storage.CurrentContext, to, to_value + value);
            
            // Calls the event to public publish the transfer
            Transferred(from, to, value);
            return true;
        }

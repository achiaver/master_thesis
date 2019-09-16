// Token Generator smart contract (TGsm)
using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;
using Neo.SmartContract.Framework.Services.System;
using System;
using System.ComponentModel;
using System.Numerics;

namespace Neo.SmartContract
{
    public class Token_Generator : Framework.SmartContract
    {
        /************************
         * Variables Definition *
         ***********************/
        // Token Settings
        public static readonly string Name = "Sharing Electricity in Brazil";
        public static readonly string Symbol = "SEB";
        public static byte[] Issuer() => ExecutionEngine.CallingScriptHash; // returns the scripthash of the caller of the contract. ? mas o valor NUNCA bate com o address do owner...
        // public static readonly byte[] Owner = operation.AsByteArray().ToScriptHash();
        public static readonly byte[] Owner = "ATrzHaicmhRj15C3Vv6e6gLfLqhSD2PtTr".ToScriptHash(); // mudar para pegar o address de quem deu o deploy
        
        // issuer? --> quem chama o contrato
        // owner? --> quem deu o deploy
        
        Mudar o Owner para uma função ou contrato...
        Só se gera tokens em dois casos:
        1. Se for uma proposta para uma nova usina (VSM --> NPP)
        2. Se for uma votação para aceitar alguém (VSM --> NM == new member)
        
        /****************
         * Basic Events *
         ****************/
        [DisplayName("transfer")] //<from_address, to_address, amount_value>
        public static event Action<byte[], byte[], BigInteger> Transferred;
        
        [DisplayName("creation")] //<issuer_address, amount_value>
        public static event Action<byte[], BigInteger> Created;
        
        /******************
         * Main Operation *
         ******************/
        public static Object Main(string operation, params object[] args)
        {
            if (Runtime.Trigger == TriggerType.Verification) // Deploy --> read only :: ainda não vi isso na AVM e no RPC
            {
                // if param Owner type is script hash...
                if (Owner.Length == 20)
                {
                    // returns the script hash itself? with 20 bytes long
                    return Runtime.CheckWitness(Owner);
                }
                // if param Owner type is public key...
                else if (Owner.Length == 33)
                {
                    // returns the script hash correspondent to the public key? with 20 bytes long
                    byte[] signature = operation.AsByteArray();
                    return VerifySignature(signature, Owner);
                }
            }
            else if (Runtime.Trigger == TriggerType.Application) // Invoke --> read/write
            {
                if (operation == "summary") return Summary();
                if (operation == "creation")
                {
                    if (args.Length != 2) return false;
                    
                    BigInteger value = (BigInteger)args[0];
                    bool auth = (bool)args[1]; // isso não é muito seguro... Esse valor virá de outro SC --> representa o consenso do grupo em se criar mais tokens (iniciar uma nova usina). --> como "trancar" esta operação com outro contrato? A partir do scriptHash dos outros contratos?
                    
                    if (auth) return Create(value);
                    return false;
                }
            }
            
            return false;
        }

        /************************
         * Functions Definition *
         ************************/
        public static string Summary()
        {
            /*
            * Returns a string with a brief description about the token status.
            * The static values of variables can be accessed through the AVM
            * log with no additional cost.
            */
            
            // Runtime.Notify("The token " + Name + " has the symbol " + Symbol + " and it has an amount of " + TotalSupplied().ToString + " distributed among members."); // deploy funciona, mas invoke não
            return Name + Symbol; // funciona, mas a união das strings não funciona
            
            // Runtime.Notify("The last operation to create new tokens was issued by {}.".(Issuer.ToString));
        }
        
        public static void Create(BigInteger amount)
        {
            BigInteger total = TotalSupplied();
            
            // STEP 1: check limits, so, I can create more tokens
            if (total > 0 & total <= 5000000) // % MW
            {
                Created(Issuer, amount);
                
                // STEP 2: Lock the amount created until the NPP operation has finished
                function Lock...
                
                // After the unlock...
                // STEP 3: Send the amount created to the contract owner to be later distributed
                distributor = Storage.Get(Owner).AsBigInteger(); // storage of TG space
                if (distributor != 0) return false; // must be an error...
                Storage.Put(Owner, amount);
                Transferred(null, Owner, amount);
                
                // STEP 4: update the total value supplied by the smart contract
                Storage.Put("totalSupplied", total + amount);
                
                // STEP 4: distribute the tokens
                calls TEsm...
                Distributed(Owner, [...list of members...], [...list of quotas...]);
                
                // STEP 5: update quota values of each member
                Isso vai ser foda!
            }
        }
        
        private static BigInteger TotalSupplied() //OK
        {
            /*
            * Returns the total value already created by this smart contract.
            * If nothing has been created, returns a null value of type ByteArray.
            */
            return Storage.Get("totalSupplied").AsBigInteger();
        }
    }
}









operations.cs
using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;
using Neo.SmartContract.Framework.Services.System; // required for 'ExecutionEngine'
using System;
using System.ComponentModel;
using System.Numerics;

namespace Neo.SmartContract
{
    public class ICO_Template : Framework.SmartContract
    {
        // Token Settings
        public static string Name = "name of the token";
        public static readonly byte[] Owner = "AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y".ToScriptHash();
        
        [DisplayName("print")]
        // public static event Action<string> Printed;
        public static event Action<TransactionOutput> Printed;

        public static Object Main(string operation, params object[] args)
        {
            if (Runtime.Trigger == TriggerType.Verification)
            {
                // Triggered when sending a transfer --> não consigo ver isso acontecendo!
                // Printed("Verification trigger");
                if (Owner.Length == 20)
                {
                    // if param Owner is script hash
                    return Runtime.CheckWitness(Owner);
                }
                else if (Owner.Length == 33)
                {
                    // if param Owner is public key
                    byte[] signature = operation.AsByteArray();
                    return VerifySignature(signature, Owner);
                }
            }
            else if (Runtime.Trigger == TriggerType.Application)
            {
                // Triggered when sending InvocationTransaction
                // Printed("Application trigger");
                if (operation == "name") return Name();
                if (operation == "test") return Testando();
                if (operation == "sender") return GetSender();
            }
            return false;
        }
        
        public static byte[] Testando()
        {
            // get smart contract script hash
            return ExecutionEngine.ExecutingScriptHash; // retorna valores estranhos... AGYKXGUgUKAJhdWXeL1rRT4gimD5AdyiLe
            // return ExecutionEngine.CallingScriptHash; // retorna valores estranhos... AJu4kasUo12gqMxifaDEdPKFUbyAfKBsuz
            // return ExecutionEngine.EntryScriptHash; // retorna valores estranhos... ALk4WH6UwyGbwM1inruKz8n4ncVLPMejoa
            
            // get sender script hash
            // Transaction tx = (Transaction)ExecutionEngine.ScriptContainer;
            // TransactionOutput[] reference = tx.GetReferences();
            // return reference[0].ScriptHash; // 
            
            // get ?? scripthash do contrato?
            // return ExecutionContext.Script; // ExecutionContext does not exist in the current context...
        }
        
        public static byte[] GetSender()
        {
            Transaction tx = (Transaction)ExecutionEngine.ScriptContainer;
            TransactionOutput[] reference = tx.GetReferences();
            // you can choice refund or not refund
            
            Printed(reference);
            
            foreach (TransactionOutput output in reference)
            {
                return output.ScriptHash;
            }
            // return new byte[]{};
            return reference[0].ScriptHash;
        }
    }
}

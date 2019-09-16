using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;
using Neo.SmartContract.Framework.Services.System;
using System;
using System.ComponentModel;
using System.Numerics;

namespace Neo.SmartContract
{
    public class NomeClasse : Framework.SmartContract
    {
        public static readonly string pubkey = "AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y";//.ToScriptHash();
        
        public static object Main(string operation, params object[] args)
        {
            if ( Runtime.Trigger == TriggerType.Verification ) // only once at deploy
            {
                return true;
            }
            else if ( Runtime.Trigger == TriggerType.Application )
            {
                if (operation == "create") return create();
                
                if (operation == "save") return save();
                
                if (operation == "get") return get( (string)args[0] );
                // if (operation == "get") return get( args[0] );
                
                return false;
            }
            
            return false;
        }

        public static string create()
        {
            Memb m = new Memb {
                                FN = "João das Couves",
                                U = "ENEL-CE",
                                Q = 100,
                                T = 0
                            };
            
            // Storage.Put(pubkey.ToScriptHash(), m.ToScriptHash()); // Erro pq só funciona com string!
                            
            return "Criado";
        }
        
        public static string get(string key)
        {
            // return Storage.Get(Storage.CurrentContext, key).AsString(); // funciona somente com o H,W!
            return Storage.Get(Storage.CurrentContext, key+"\x00"+"FullName").AsString(); // funciona somente com o StorageMap!
            
            // Runtime.Notify(member.Get("FullName").AsString()); // retorna João das Couves
            // Runtime.Notify( Storage.Get(Storage.CurrentContext, PubKey) ); // deu ruim :/
            // Runtime.Notify(Storage.Get(Storage.CurrentContext, "user\x00xyz").AsBigInteger()); // will print '20'
        }
        
        public static string save()
        {
            // writes value "World" on storage key "Hello"
            // implicitly calls Storage.Put(Storage.CurrentContext, "Hello", "World")
            Storage.Put("Hello", "World"); // funciona!
            
            // string pubkey = "AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y";//.ToScriptHash();
            Member( pubkey, "João das Couves", "ENEL-CE", 100, 0 );
            // AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y\x00FullName
            
            return "Salvo";
        }
        
        private static void Member(string PubKey, string FullName, string Utility, BigInteger Quota, BigInteger Tokens)
        {
            StorageMap member = Storage.CurrentContext.CreateMap( PubKey );
            member.Put( "FullName", FullName );
            member.Put( "Utility", Utility );
            member.Put( "Quota", Quota );
            member.Put( "Tokens", Tokens );
        }
        
        private struct Memb // a princípio não dá pra salvar struct, nem convertê-lo para hash...
        {
            public string FN;
            public string U;
            public BigInteger Q;   // from 0 to 100
            public BigInteger T;  // must be 0 by default
        }
    }
}

// Runtime.Notify(user_sm.Get("xyz").AsBigInteger()); // will print '20'
// Runtime.Notify(Storage.Get(Storage.CurrentContext, "user\x00xyz").AsBigInteger()); // will print '20'

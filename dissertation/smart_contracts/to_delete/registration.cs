// Domain = User registration?
// https://docs.neo.org/en-us/sc/tutorial/Domain.html

// DIGITAL IDENTITY

// posso usar struct?!
// Para usar a artemanha do 'Hello, World'? (identificar cada membro por seu nome? quais problemas podem acontecer? Seria melhor o CPF ou CNPJ?)
// writes value "World" on storage key "Hello"
// implicitly calls Storage.Put(Storage.CurrentContext, "Hello", "World")

using Neo.SmartContract.Framework;
using Neo.SmartContract.Framework.Services.Neo;

namespace Neo.SmartContract
{
    // Contract Owner --> main address at neocompiler
    byte[] Owner = "AK2nJJpJr6o664CWJKi1QRXjqeic2zRp8y".ToScriptHash();
    private bool has_fullAccess = Runtime.CheckWitness(Owner);
    // Limit some functions to be triggered out
    // contract name: Group Registration
    // contract author: Yuri Bastos Gabrich
    // contract version: 1.0
    // contact email: about.me/yuribg
    
    // Events
    [DisplayName("Notification:\t")]
    public static event Action<byte[], string> Notification;
    
    // Initial condition (run only once on the smart contract lifetime)
    // Pq preciso disso? Para definir o owner?
    private static bool first_run;
    if (!has_fullAccess)
    {
        first_run = false;
    } else {
        // Como eu sei que já executei esse contrato se eu não tenho o 'smart contract account'?
        // Preciso procurar pelo contrato na ledger? Isso custa quanto?
        // Como eu sei quem é o Owner, posso encontrar o deploy do contrato? Ver exemplo.cs (não funciona...)
    }
    
    // Define structs
    private struct Stakeholder
    {
        public string bizName;          // nome fantasia da distribuidora, do órgão
                                        // legislador ou do grupo
        public BigInteger powerCapacity;// somatório das potências da usinas
        public bool is_group;           // condição para agregar membros
        public bool is_utility;         // condição para regular membros
    
        private Stakeholder(string _bizName, BigInteger _powerCapacity, bool _is_group, bool _is_utility)
        {
            bizName = _bizName;
            powerCapacity = _powerCapacity;
            is_group = _is_group;
            is_utility = _is_utility;
        }
    }
    
    private struct Member
    {
        public string ID;               // CPF ou CNPJ (public key?)
        public string fullName;         // pessoa ou nome fantasia (não a razão social)
        public Stakeholder utilityName; // nome fantasia da distribuidora
        public Stakeholder groupName;   // nome do grupo que faz parte
        public BigInteger quota;        // participação do membro no grupo
        public byte[] pubKey;           // chave pública do membro
        
        public Member(string _ID, string _fullName, string _utilityName, string _groupName, BigInteger _quota, byte[] _pubKey)
        {
            ID = _ID;
            fullName = _fullName;
            utilityName = _utilityName;
            groupName = _groupName;
            quota = (_quota/100);
            pubKey = _pubKey;
        }
    }
    
    public class Domain : Framework.SmartContract
    {
        public static object Main(string operation, params object[] args)
        {
            if (first_run)
            {
                // create a 'group' with the 'owner' as a only 'member'
                Stakeholder new_group;
                Stakeholder new_utility;
                Member founder;
                
                string temp = "must be updated";
                new_group = new Stakeholder(temp,
                                            0.0,
                                            true,
                                            false);
                new_utility = new Stakeholder(temp,
                                              0.0,
                                              false,
                                              true);
                founder = new Member(temp,
                                     temp,
                                     new_utility.bizName,
                                     new_group.bizName,
                                     100.0,
                                     Owner);
                
                first_run = false;
                return false; // on the first deploy,
                              // the smart contract only starts a new group registration contract and creates a 'smart contract account' (não sei como fazer isso por aqui...)
                              // https://docs.neo.org/en-us/sc/quickstart/deploy-invoke.html
            }
            
            // other call than the first deploy
            switch (operation)
            {            
                if (!has_fullAccess)
                {
                    case "query": // returns the information storage in the contract space
                        return Query((string)args[0]);
                    case "request": // request to use the smart contract
                        return Request();
                } else {
                    case "query": // returns the information storage in the contract space
                        return Query((string)args[0]);
                    case "register": // writes new information in the contract space (once per reg)
                        return Register((string)args[0], (byte[])args[1]);
                    case "update": // rewrite informations when needed
                        return Update((string)args[0], (byte[])args[1]);
                    case "transfer": // transfer a member's quota
                        return Transfer((string)args[0], (byte[])args[1]);
                    case "move": // change the contract ownership
                        return Move((string)args[0], (byte[])args[1]); // Como fazer isso se o endereço atual está escrito na ledger e não é uma função q pega a hash de quem está escrevendo o contrato?
                    case "delete": // delete a register
                        return Delete((string)args[0]);
                    default:
                        return false;
                }
            }
        }
        
        private static byte[] Query(string domain) // retorna byte?
        {
            // returns a 'Stakeholder' or 'Member' information
            public get_args = Storage.Get(Storage.CurrentContext, domain);
            
            if (domain is Stakeholder) // não sei como verificar isso...
            // if (domain.type == Stakehoder) ?
            {
                public members = Storage.Get(Storage.CurrentContext, Member); // não sei como fazer isso
                public array gatherList; // list members by stakeholder
                
                if (domain.is_group)
                {
                    for member in members
                    {
                        if (member.groupName == domain) // bizName
                        {
                            gatherList.append(member);
                        }
                    }
                } else if (is_utility) {
                    for member in members
                    {
                        if (member.utilityName == domain) // bizName
                        {
                            gatherList.append(member);
                        }
                    }
                } else { // is legislator
                    gatherList = members;
                }
                
                return get_args, gatherList;
            
            } else { // is a Member
                return get_args;
            }
        }

        private static bool Request()
        {
            return false;
        }
        
        private static bool Register(string domain, byte[] owner, params object[] args)
        {
            // check if the owner of the contract is calling this function
            // if (!Runtime.CheckWitness(owner)) return false;
            
            // check if the domain already exist in the contract storage space
            byte[] value = Storage.Get(Storage.CurrentContext, domain);
            if (value != null)
            {
                emit Notification(domain, "This domain is already registered on the smart contract. Raise 'Query' option."); // quanto custa isso?
                return false;
            }
            
            // add a new domain
            // raise contract 'poll' to vote for the integration of a new data
            if poll // será que consigo chamar pelo nome ou preciso passar o endereço? Talvez seja preciso definir isso no first_run!
            {
                Storage.Put(domain, args...);
                return true;
            }
            
            return true;
        }

        private static bool Transfer(string domain, byte[] to)
        {
            if (!Runtime.CheckWitness(to)) return false;
            byte[] from = Storage.Get(Storage.CurrentContext, domain);
            if (from == null) return false;
            if (!Runtime.CheckWitness(from)) return false;
            Storage.Put(Storage.CurrentContext, domain, to);
            return true;
        }

        private static bool Move(string domain, byte[] to)
        {
            
        }
        
        private static bool Delete(string domain)
        {
            byte[] owner = Storage.Get(Storage.CurrentContext, domain);
            if (owner == null) return false;
            if (!Runtime.CheckWitness(owner)) return false;
            Storage.Delete(Storage.CurrentContext, domain);
            return true;
        }
    }
}
// Define the hierarchy of the members and, consequently, their permission to access some data

// 'member' and 'group' has different access to the data
        private bool has_fullAccess(byte[] caller)
        {
            if (!Runtime.CheckWitness( (caller.type == Member) || (caller.type == Stakeholder.is_group) ))
            {
                return true;
            } else {
                return false;
            }
        }


// DIGITAL IDENTITY
using Group.Assets;

namespace Hierarchy
{
    public class Level : SmartContract
    {
        private static readonly byte[] Owner = groupPBkey.ToScriptHash();
        
        [DisplayName("access")]
        public static event Action<byte[], string> ??;
        
        public static object setAccess(string method, object[] args)
        {
            // Owner set the register data of 'to' a access tag
            // var ?

            if (method == "register") return AccessOf((byte[])args[0], (string)args[1]);
            
            if (method == "update")
            {
                if ( (byte[])args[0].Hierarchy.Level == (string)args[1] ):
                    // show a notification that the new value is the same as the old one
                    return void
                else:
                    return AccessOf((byte[])args[0], (string)args[1]);
            }
        }
            
        [DisplayName("permission")]
        public static string AccessOf(byte[] to, string level)
        {
            // write on the ledger the permission level of a member of the group
        }
        
    }
}
// Ethereum code: https://www.ethereum.org/crowdsale --> (como distribuir os tokens?)
// =====================================================================================================

using trade.cs

// interface token {
namespace newDGplant // a Contract Asset? Nope, a smart contract API to an contract asset
{

    function transfer(byte[] to, BigInteger amount) external;

    // contract Crowdsale {
    public class Crowdsale : SmartContract
    {
    
        // define variables
        byte[] public beneficiary; // the group
        BigInteger public fundingGoal; // reais?
        BigInteger public amountRaised;  // reais?
        BigInteger public deadline;
        BigInteger public price;
        token public tokenReward;
        // mapping(byte[] => BigInteger) public balanceOf;
        trade.cs.BalanceOf(byte[])
        bool fundingGoalReached = false;
        bool crowdsaleClosed = false;
        
        // Events
        [DisplayName("Goal Reached")]
        public static event Action<byte[], BigInteger> GoalReached;
        // event GoalReached(byte[] recipient, BigInteger totalAmountRaised);
        
        [DisplayName("Transaction")]
        public static event Action<byte[], BigInteger, bool> FundTransfer;
        // event (byte[] backer, BigInteger amount, bool isContribution);
    
        // Setup the owner of the tokens
        public static ?? Constructor(byte[] ifSuccessfulSendTo,
                                        BigInteger fundingGoalInEthers,
                                        BigInteger durationInMinutes,
                                        BigInteger etherCostOfEachToken,
                                        byte[] byte[]OfTokenUsedAsReward)
        {
            beneficiary = ifSuccessfulSendTo;
            fundingGoal = fundingGoalInEthers * 1 ether;
            deadline = now + durationInMinutes * 1 minutes;
            price = etherCostOfEachToken * 1 ether;
            tokenReward = token(byte[]OfTokenUsedAsReward);
        }
    
        /**
         * Fallback function
         *
         * The function without name is the default function that is called whenever anyone sends funds to a contract
         */
        function () payable external {
            require(!crowdsaleClosed);
            BigInteger amount = msg.value;
            balanceOf[msg.sender] += amount;
            amountRaised += amount;
            tokenReward.transfer(msg.sender, amount / price);
           emit FundTransfer(msg.sender, amount, true);
        }
    
        modifier afterDeadline() { if (now >= deadline) _; }
    
        /**
         * Check if goal was reached
         *
         * Checks if the goal or time limit has been reached and ends the campaign
         */
        [DisplayName("Goal Reached ?")]
        public static bool checkGoalReached()
        {
            if afterDeadline => crowdsaleClosed = true;
            
            if (amountRaised >= fundingGoal)
            {
                fundingGoalReached = true;
                emit GoalReached(beneficiary, amountRaised);
                return true;
            }
            return false;
        }
    
        /**
         * Withdraw the funds
         *
         * Checks to see if goal or time limit has been reached, and if so, and the funding goal was reached,
         * sends the entire amount to the beneficiary. If goal was not reached, each contributor can withdraw
         * the amount they contributed.
         */
        [DisplayName("Check Goal")]
        public static object checkGoalReached()
        {
            if (afterDeadline)
            {
                if (!fundingGoalReached)
                {
                    BigInteger amount = balanceOf[msg.sender];
                    balanceOf[msg.sender] = 0;
                    if (amount > 0)
                    {
                        if (msg.sender.send(amount))
                        {
                           emit FundTransfer(msg.sender, amount, false);
                        } else {
                            balanceOf[msg.sender] = amount;
                        }
                    }
                }
            }
            if (fundingGoalReached && beneficiary == msg.sender)
            {
                if (msg.sender.send(amountRaised))
                {
                   emit FundTransfer(beneficiary, amountRaised, false);
                } else {
                    //If we fail to send the funds to beneficiary, unlock funders balance
                    fundingGoalReached = false;
                }
            }
        }
    }
}

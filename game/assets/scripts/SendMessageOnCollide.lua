function OnCollisionEnter(other)
	other:SendMessage("TestMessageFunction", 1, 2, "three", this)
end
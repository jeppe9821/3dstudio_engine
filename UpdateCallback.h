#pragma once

class Node;

class UpdateCallback
{
	public:
		virtual void update(Node &n) = 0;
};
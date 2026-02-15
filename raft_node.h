#pragma once
#include <vector>
#include <string>
#include <atomic>

enum class NodeState
{
    FOLLOWER,
    CANDIDATE,
    LEADER
};

class RaftNode
{
public:
    RaftNode(int id);

    void start();
    void handle_append_entries();
    void handle_request_vote();
    void become_leader();

private:
    int node_id;
    std::atomic<NodeState> state;

    int current_term;
    int voted_for;
    int commit_index;
};

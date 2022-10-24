/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifdef DEF_NAME
// define all names here(use lower case)
// @note append only, search `<<' below
DEF_NAME(id, "id")
    DEF_NAME(t, "timestamp")
    DEF_NAME(first, "first")
    DEF_NAME(second, "second")
    DEF_NAME(ret, "return code")
    DEF_NAME(key, "key")
    DEF_NAME(value, "value")
    DEF_NAME(trace_id, "trace id")
    DEF_NAME(id1, "id1")
    DEF_NAME(id2, "id2")
    DEF_NAME(id3, "id3")
    DEF_NAME(arg, "arg")
    DEF_NAME(arg1, "arg1")
    DEF_NAME(arg2, "arg2")
    DEF_NAME(arg3, "arg3")
    DEF_NAME(arg4, "arg4")
    DEF_NAME(val1, "val1")
    DEF_NAME(val2, "val2")
    DEF_NAME(val3, "val3")
    DEF_NAME(param, "param")
    DEF_NAME(tag1, "tag1")
    DEF_NAME(tag2, "tag2")
    DEF_NAME(tag3, "tag3")
    DEF_NAME(tag4, "tag4")
    DEF_NAME(tag5, "tag5")
    DEF_NAME(tag6, "tag6")
    DEF_NAME(tag7, "tag7")
    DEF_NAME(tag8, "tag8")
    DEF_NAME(tag9, "tag9")
    DEF_NAME(tag10, "tag10")
    DEF_NAME(tag11, "tag11")
    DEF_NAME(tag12, "tag12")
    DEF_NAME(tag13, "tag13")
    DEF_NAME(tag14, "tag14")
    DEF_NAME(tag15, "tag15")
    DEF_NAME(tag16, "tag16")
    DEF_NAME(tag17, "tag17")
    DEF_NAME(tag18, "tag18")
    DEF_NAME(tag19, "tag19")
    DEF_NAME(tag20, "tag20")
    DEF_NAME(ls_id, "logstream id")
    DEF_NAME(from, "from")
    DEF_NAME(to, "to")
    DEF_NAME(read_only, "read only")
    DEF_NAME(access_mode, "access_mode")
    DEF_NAME(isolation_level, "isolation level")
    DEF_NAME(thread_id, "thread id")
    DEF_NAME(type, "type")
    DEF_NAME(flag, "flag")
    DEF_NAME(null, "null")
    DEF_NAME(tenant, "tenant")
    DEF_NAME(tenant_id, "tenant id")
    DEF_NAME(db, "database")
    DEF_NAME(table, "table")
    DEF_NAME(tid, "table_id")
    DEF_NAME(cid, "column_id")
    DEF_NAME(column, "column")
    DEF_NAME(session, "session")
    DEF_NAME(sid, "session id")
    DEF_NAME(user, "user")
    DEF_NAME(partition, "partition")
    DEF_NAME(replica_type, "replica type")
    DEF_NAME(pkey, "partition key")
    DEF_NAME(pkeys, "partition keys")
    DEF_NAME(partition_id, "partition id")
    DEF_NAME(part_idx, "part index")
    DEF_NAME(part_id, "part id")
    DEF_NAME(subpart_id, "subpart_id")
    DEF_NAME(subpart_idx, "subpart_idx")
    DEF_NAME(part_cnt, "partition count")
    DEF_NAME(plan_id, "plan_id")
    DEF_NAME(affected_rows, "affected_rows")
    DEF_NAME(limit, "limit")
    DEF_NAME(offset, "offset")
    DEF_NAME(scan, "scan")
    DEF_NAME(get, "get")
    DEF_NAME(idx, "idx")
    DEF_NAME(used, "used")
    DEF_NAME(time_used, "time used")
    DEF_NAME(num, "num")
    DEF_NAME(init, "init")
    DEF_NAME(destroy, "destroy")
    DEF_NAME(exiting, "exiting")
    DEF_NAME(ref, "reference count")
    DEF_NAME(length, "length")
    DEF_NAME(precision, "precision")
    DEF_NAME(scale, "scale")
    DEF_NAME(sys_var, "system variable")
    DEF_NAME(user_var, "user variable")
    DEF_NAME(op, "operator")
    DEF_NAME(func, "function")
    DEF_NAME(timeout, "timeout")
    DEF_NAME(ip, "ip address")
    DEF_NAME(port, "port")
    DEF_NAME(addr, "address")
    DEF_NAME(pcode, "packet code")
    DEF_NAME(acquire, "acquire")
    DEF_NAME(release, "release")
    DEF_NAME(reuse, "reuse")
    DEF_NAME(result, "result")
    DEF_NAME(is_finish, "is_finish")
    DEF_NAME(before_processor_run, "before processor run")
    DEF_NAME(start_rpc, "start rpc")
    DEF_NAME(start_sql, "start sql")
    DEF_NAME(found_rows, "found rows")
    DEF_NAME(return_rows, "return rows")
    DEF_NAME(input_count, "input count")
    DEF_NAME(total_count, "total count")
    DEF_NAME(last_insert_id, "last insert id")
    DEF_NAME(receive, "receive")
    DEF_NAME(transmit, "transmit")
    DEF_NAME(remote_result, "remote result")
    DEF_NAME(process_ret, "process return code")
    DEF_NAME(stmt, "statement")
    DEF_NAME(stmt_len, "statement length")
    DEF_NAME(store_found_rows, "store found rows")
    DEF_NAME(start_trans, "start transaction")
    DEF_NAME(register_timeout_task, "register timeout task")
    DEF_NAME(unregister_timeout_task, "unregister timeout task")
    DEF_NAME(start_part, "start participant")
    DEF_NAME(trans_id, "transaction id")
    DEF_NAME(local_task_completed, "local task completed")
    DEF_NAME(remote_task_completed, "remote task completed")
    DEF_NAME(distributed_task_submited, "distributed task submited")
    DEF_NAME(task, "task")
    DEF_NAME(hash, "hash")
    DEF_NAME(inc, "inc")
    DEF_NAME(scanner, "scanner")
    DEF_NAME(wait_start, "wait start")
    DEF_NAME(wait_end, "wait end")
    DEF_NAME(job_id, "job id")
    DEF_NAME(task_id, "task id")
    DEF_NAME(remote_sql, "remote sql")
    DEF_NAME(runner_svr, "runner server")
    DEF_NAME(ctrl_svr, "scheduler server")
    DEF_NAME(execution_id, "execution id")
    DEF_NAME(lock_row, "lock row")
    DEF_NAME(execute_task, "execute task")
    DEF_NAME(stmt_type, "statement type")
    DEF_NAME(execute_async_task, "execute async task")
    DEF_NAME(check_priv, "check privilege")
    DEF_NAME(result_set_close, "result set close")
    DEF_NAME(async, "async")
    DEF_NAME(row_count, "row count")
    DEF_NAME(post_packet, "post packet")
    DEF_NAME(revert_scan_iter, "revert scan iterator")
    DEF_NAME(get_row, "get row")
    DEF_NAME(plc_sys_cache_get, "plc sys cache get end")
    DEF_NAME(plc_user_cache_get_end, "plc user cache get end")
    DEF_NAME(tl_calc_parid_end, "tl calc part id end")
    DEF_NAME(tl_calc_by_range_end, "tl calc by range end")
    DEF_NAME(pc_fast_parse_start, "pc fast parse start")
    DEF_NAME(pc_fast_parse_end, "pc fast parse end")
    DEF_NAME(pc_normal_parse_start, "pc normal parse stat")
    DEF_NAME(pc_normal_parse_end, "pc normal parse end")
    DEF_NAME(pc_get_stmt_key_start, "pc get stmt key start")
    DEF_NAME(pc_get_stmt_key_end, "pc get stmt key end")
    DEF_NAME(cache_get_value, "pc get value end")
    DEF_NAME(cache_update_stat, "pcv update stmt stat")
    DEF_NAME(pc_choose_plan, "pc choose plan end")
    DEF_NAME(calculate_type_end, "pc calc type end")
    DEF_NAME(get_plan_type_end, "pc get plan type end")
    DEF_NAME(trav_ps_map_start, "trav_ps_map_start")
    DEF_NAME(trav_ps_map_end, "trav_ps_map_end")
    DEF_NAME(tl_calc_part_id_end, "tl calc part id end")
    DEF_NAME(cons_context, "construct context")
    DEF_NAME(handle_message, "handle message")
    DEF_NAME(alloc_redo_log_sync_task, "alloc_redo_log_sync_task")
    DEF_NAME(retry_redo_log_sync_task, "retry_redo_log_sync_task")
    DEF_NAME(handle_slow_message, "handle slow message")
    DEF_NAME(handle_all_message, "handle all message")
    DEF_NAME(msg, "message")
    DEF_NAME(msg_type, "message type")
    DEF_NAME(handle_timeout, "handle timeout")
    DEF_NAME(start_participant, "start_participant")
    DEF_NAME(end_participant, "end_participant")
    DEF_NAME(kill, "kill")
    DEF_NAME(scheduler, "scheduler")
    DEF_NAME(trans_version, "transaction version")
    DEF_NAME(part_start_trans, "partition start transaction")
    DEF_NAME(insert_tx_data, "insert tx data to tx table")
    DEF_NAME(part_end_trans, "partition end transaction")
    DEF_NAME(left_time, "left time")
    DEF_NAME(is_rollback, "is rollback")
    DEF_NAME(log_sync_succ_cb, "on_log_sync_success")
    DEF_NAME(leader_takeover, "leader takeover")
    DEF_NAME(leader_active, "leader active")
    DEF_NAME(leader_revoke, "leader revoke")
    DEF_NAME(switch_to_follower_forcely, "switch to follower forcely")
    DEF_NAME(switch_to_follower_gracefully, "switch to follower forcely")
    DEF_NAME(switch_to_leader, "switch to leader")
    DEF_NAME(resume_leader, "resume leader")
    DEF_NAME(need_release, "need release")
    DEF_NAME(recover_from_ctx_table, "recover from ctx table")
    DEF_NAME(max_applying_ts, "max applying log ts")
    DEF_NAME(skip_replay_redo, "skip replay redo log")
    DEF_NAME(replay_redo, "replay redo log")
    DEF_NAME(replay_prepare, "replay prepare log")
    DEF_NAME(replay_commit, "replay commit log")
    DEF_NAME(replay_abort, "replay abort log")
    DEF_NAME(replay_clear, "replay clear log")
    DEF_NAME(replay_trans_state, "replay trans state log")
    DEF_NAME(replay_trans_mutator, "replay trans mutator log")
    DEF_NAME(replay_commit_info, "replay commit info log")
    DEF_NAME(replay_multi_data_source, "replay multi data source log")
    DEF_NAME(replay_record, "replay record log")
    DEF_NAME(need_replay, "need replay")
    DEF_NAME(log_pending, "log pending")
    DEF_NAME(log_pending_cb, "log pending callback")
    DEF_NAME(submit_log, "submit log")
    DEF_NAME(submit_instant_log, "submit instant log")
    DEF_NAME(submit_rollback_log, "submit rollback log")
    DEF_NAME(retry_submit_log, "retry submit log")
    DEF_NAME(submit_log_cb, "submit log callback")
    DEF_NAME(submit_commit, "submit commit")
    DEF_NAME(submit_abort, "submit abort")
    DEF_NAME(update_trans_version, "update transaction version")
    DEF_NAME(prepare, "prepare")
    DEF_NAME(ctx_ref, "transaction context reference")
    DEF_NAME(on_submit_log_succ_cb, "on submit log succ cb")
    DEF_NAME(on_fail_cb, "on failure cb")
    DEF_NAME(submit_log_count, "submit log count")
    DEF_NAME(submit_log_pending_count, "submit log pending count")
    DEF_NAME(logging, "submit log count")
    DEF_NAME(pending, "submit log pending count")
    DEF_NAME(log_type, "log type")
    DEF_NAME(log_no, "log number")
    DEF_NAME(lsn, "log sequence number")
    DEF_NAME(is_readonly, "is readonly")
    DEF_NAME(recover_dist_trans, "recover dist trans")
    DEF_NAME(alloc_logid_ts, "alloc logid and timestamp")
    DEF_NAME(trans_type, "transaction type")
    DEF_NAME(end_trans_cb, "end trans callback")
    DEF_NAME(publish_version, "publish version")
    DEF_NAME(submit_recommit, "submit recommit")
    DEF_NAME(submit_reabort, "submit reabort")
    DEF_NAME(submit_recreate, "submit recreate")
    DEF_NAME(start_stmt, "submit stmt")
    DEF_NAME(start_end_stmt, "start_end_stmt")
    DEF_NAME(end_stmt, "end stmt")
    DEF_NAME(get_store_ctx, "get store ctx")
    DEF_NAME(end_trans, "end transaction")
    DEF_NAME(wait_get_gts, "wait get gts")
    DEF_NAME(wait_gts_elapse, "wait gts elapse")
    DEF_NAME(gts_callback, "gts callback")
    DEF_NAME(get_gts_callback, "get gts callback")
    DEF_NAME(gts_elapse_callback, "gts elapse callback")
    DEF_NAME(inc_submit_log_count, "inc submit log count")
    DEF_NAME(dec_submit_log_count, "dec submit log count")
    DEF_NAME(retry_cnt, "retry count")
    DEF_NAME(opid, "operation id")
    DEF_NAME(txid, "trx id")
    DEF_NAME(start_tx, "start tx")
    DEF_NAME(rollback_tx, "rollback tx")
    DEF_NAME(abort, "abort")
    DEF_NAME(abort_tx, "abort tx")
    DEF_NAME(reason, "reason")
    DEF_NAME(tx_ctx_gc, "txn ctx gc")
    DEF_NAME(commit_tx, "commit tx")
    DEF_NAME(submit_commit_tx, "submit commit tx")
    DEF_NAME(handle_tx_commit_result, "handle tx commit result")
    DEF_NAME(exec_commit_cb, "exec commit callback")
    DEF_NAME(get_read_snapshot, "get read snapshot")
    DEF_NAME(get_ls_read_snapshot, "get ls read snapshot")
    DEF_NAME(register_snapshot, "register snapshot to txn")
    DEF_NAME(unregister_snapshot, "register snapshot to txn")
    DEF_NAME(release_snapshot, "release snapshot")
    DEF_NAME(start_epoch, "start epoch")
    DEF_NAME(create_implicit_savepoint, "create implicit savepoint")
    DEF_NAME(create_local_implicit_savepoint, "create local implicit savepoint")
    DEF_NAME(create_global_implicit_savepoint, "create global implicit savepoint")
    DEF_NAME(rollback_implicit_savepoint, "rollback implicit savepoint")
    DEF_NAME(rollback_local_implicit_savepoint, "rollback local implicit savepoint")
    DEF_NAME(rollback_global_implicit_savepoint, "rollback global implicit savepoint")
    DEF_NAME(create_explicit_savepoint, "create explicit savepoint")
    DEF_NAME(rollback_explicit_savepoint, "rollback explicit savepoint")
    DEF_NAME(release_explicit_savepoint, "release explicit savepoint")
    DEF_NAME(create_stash_savepoint, "create stash savepoint")
    DEF_NAME(merge_tx_state, "merge tx state")
    DEF_NAME(add_tx_exec_result, "add tx exec result")
    DEF_NAME(migrate_trx, "migrate trx")
    DEF_NAME(savepoint, "create savepoint")
    DEF_NAME(rollback_savepoint, "rollback savepoint")
    DEF_NAME(rollback_savepoint_slowpath, "rollback savepoint slowpath")
    DEF_NAME(release_savepoint, "release savepoint")
    DEF_NAME(timeguard, "time guard")
    DEF_NAME(srr, "send rpc request")
    DEF_NAME(stc, "start to commit")
    DEF_NAME(set_stc, "set stc")
    DEF_NAME(set_stc_by_get, "set stc by get")
    DEF_NAME(gts, "global timestamp")
    DEF_NAME(sender, "sender")
    DEF_NAME(election, "election")
    DEF_NAME(new_period, "election new period")
    DEF_NAME(election_init, "election init")
    DEF_NAME(lease_expired, "leader lease expired")
    DEF_NAME(expire_ts, "expire timestamp")
    DEF_NAME(reappoint, "reappoint")
    DEF_NAME(reappoint_count, "reappoint count")
    DEF_NAME(vote_period, "vote period")
    DEF_NAME(lease_time, "lease time")
    DEF_NAME(cur_leader, "current leader")
    DEF_NAME(new_leader, "new leader")
    DEF_NAME(priority, "priority")
    DEF_NAME(ticket, "ticket")
    DEF_NAME(valid_candidates, "valid candidates")
    DEF_NAME(change_leader_timestamp, "change leader timestamp")
    DEF_NAME(ignore_log, "ignore log")
    DEF_NAME(send_timestamp, "send timestamp")
    DEF_NAME(leader, "leader")
    DEF_NAME(vote_leader, "vote leader")
    DEF_NAME(T1_timestamp, "T1 timestamp")
    DEF_NAME(t1, "t1")
    DEF_NAME(epoch, "epoch")
    DEF_NAME(is_candidate, "is candidate")
    DEF_NAME(membership_version, "membership version")
    DEF_NAME(log_id, "log id")
    DEF_NAME(data_version, "data version")
    DEF_NAME(version, "version")
    DEF_NAME(server, "server")
    DEF_NAME(is_running, "is running")
    DEF_NAME(is_offline, "is offline")
    DEF_NAME(is_changing_leader, "is changing leader")
    DEF_NAME(self, "self")
    DEF_NAME(proposal_leader, "proposal leader")
    DEF_NAME(curr_candidates, "current candidates")
    DEF_NAME(curr_membership_version, "curr_membership_version")
    DEF_NAME(replica_num, "replica number")
    DEF_NAME(leader_epoch, "leader epoch")
    DEF_NAME(leader_lease, "leader lease")
    DEF_NAME(election_time_offset, "election_time_offset")
    DEF_NAME(active_timestamp, "active_timestamp")
    DEF_NAME(state, "state")
    DEF_NAME(role, "role")
    DEF_NAME(stage, "stage")
    DEF_NAME(member, "member")
    DEF_NAME(unconfirmed_leader, "unconfirmed_leader")
    DEF_NAME(unconfirmed_leader_lease, "unconfirmed_leader_lease")
    DEF_NAME(is_need_query, "is_need_query")
    DEF_NAME(takeover_t1_timestamp, "takeover_t1_timestamp")
    DEF_NAME(leader_revoke_timestamp, "leader_revoke_timestamp")
    DEF_NAME(move_out_timestamp, "move_out_timestamp")
    DEF_NAME(locality, "locality")
    DEF_NAME(click, "click")
    DEF_NAME(start_ts, "start timestamp")
    DEF_NAME(last_ts, "last timestamp")
    DEF_NAME(in_queue_time, "in queue time")
    DEF_NAME(receive_ts, "receive packet timestamp")
    DEF_NAME(enqueue_ts, "enqueue timestamp")
    DEF_NAME(run_ts, "run timestamp")
    DEF_NAME(expect_ts, "expect ts")
    DEF_NAME(handle_devote_prepare, "handle devote prepare")
    DEF_NAME(handle_devote_vote, "handle devote vote")
    DEF_NAME(handle_devote_success, "handle devote success")
    DEF_NAME(handle_vote_prepare, "handle vote prepare")
    DEF_NAME(handle_vote_vote, "handle vote vote")
    DEF_NAME(handle_vote_success, "handle vote success")
    DEF_NAME(handle_query_leader, "handle query leader")
    DEF_NAME(handle_query_leader_response, "handle query leader response")
    DEF_NAME(send_devote_prepare, "send devote prepare")
    DEF_NAME(send_vote_prepare, "send vote prepare")
    DEF_NAME(send_change_leader_vote_prepare, "send change leader vote prepare")
    DEF_NAME(send_query_leader, "send query leader")
    DEF_NAME(get_decentralized_candidate, "get decentralized candidate")
    DEF_NAME(get_centralized_candidate, "get centralized candidate")
    DEF_NAME(send_devote_vote, "send devote vote")
    DEF_NAME(send_vote_vote, "send vote vote")
    DEF_NAME(send_devote_success, "send devote success")
    DEF_NAME(send_vote_success, "send vote success")
    DEF_NAME(try_centrialized_voting, "try centrialized voting")
    DEF_NAME(check_decentrialized_majority, "check decentrialized majority")
    DEF_NAME(check_centrialized_majority, "check centrialized majority")
    DEF_NAME(upgrade_mode, "upgrade mode")
    DEF_NAME(run_gt1_task, "run gt1 task")
    DEF_NAME(response_scheduler, "response scheduler")
    DEF_NAME(phy_plan_type, "phy plan type")
    DEF_NAME(elr_prev_next_callback, "elr prev next callback")
    DEF_NAME(start_access, "start access")
    DEF_NAME(register_multi_data_source, "register multi data source")
    DEF_NAME(pending_cnt, "pending")
    DEF_NAME(end_access, "end access")
    DEF_NAME(sql_no, "sql number")
    DEF_NAME(seq_no, "sequence number")
    DEF_NAME(data_seq, "data sequence number")
    DEF_NAME(sql_id, "sql id")
    DEF_NAME(rescan, "rescan")
    DEF_NAME(range, "range")
    DEF_NAME(cluster_version, "cluster version")
    DEF_NAME(cluster_id, "cluster id")
    DEF_NAME(ctx_type, "context type")
    DEF_NAME(time_to_usec, "time to usec")
    DEF_NAME(start_timestamp, "start_timestamp")
    DEF_NAME(eg_id, "eg id")
    DEF_NAME(sorted_member_list, "sorted member list")
    DEF_NAME(create_time, "create time")
    DEF_NAME(lease_start, "lease start")
    DEF_NAME(lease_end, "lease end")
    DEF_NAME(election_group, "election group")
    DEF_NAME(eg_id_hash, "eg_id_hash")
    DEF_NAME(eg_part_array_idx, "eg_part_array_idx")
    DEF_NAME(election_group_init, "election group init")
    DEF_NAME(eg_part_cnt, "eg_part_cnt")
    DEF_NAME(eg_version, "eg_version")
    DEF_NAME(is_all_part_merged_in, "is_all_part_merged_in")
    DEF_NAME(pre_destroy_state, "pre_destroy_state")
    DEF_NAME(system_score, "system_score")
    DEF_NAME(is_gt_task_stopped, "is_gt_task_stopped")
    DEF_NAME(checkpoint, "checkpoint")
    DEF_NAME(commit_version, "commit_version")
    DEF_NAME(cb_type, "callback type")
    DEF_NAME(handle_batch_commit_succ, "handle_batch_commit_succ")
    DEF_NAME(generate_snapshot_version, "generate_snapshot_version")
    DEF_NAME(generate_redo_prepare_log, "generate_redo_prepare_log")
    DEF_NAME(request_id, "request_id")
    DEF_NAME(memtable, "memtable")
    DEF_NAME(remove_callback_for_uncommitted_txn, "remove_callback_for_uncommitted_txn")
    DEF_NAME(reboot, "reboot")
    DEF_NAME(calc_checksum_by_rollback, "calc_checksum_by_rollback")
    DEF_NAME(calc_checksum_by_minor_freeze, "calc_checksum_by_minor_freeze")
    DEF_NAME(calc_checksum_by_commit, "calc_checksum_by_commit")
    DEF_NAME(checksum, "checksum")
    DEF_NAME(checksum_log_ts, "checksum_log_ts")
    //clog
    DEF_NAME(follower_active_to_reconfirm, "follower active to reconfirm")
    DEF_NAME(on_leader_takeover_finish, "on leader takeover finish")
    DEF_NAME(reconfirm_to_takeover, "reconfirm to leader takeover")
    DEF_NAME(on_leader_active_finish, "on leader active finish")
    DEF_NAME(takeover_to_leader_active, "leader takeover to leader active")
    DEF_NAME(leader_active_to_revoke, "leader active to follower revoke")
    DEF_NAME(on_leader_revoke_finish, "on leader revoke finish")
    DEF_NAME(revoke_to_follower_active, "follower revoke to follower active")
    DEF_NAME(call_reconfirm, "call reconfirm")
    DEF_NAME(reconfirm_waiting_retry, "reconfirm waiting retry")
    DEF_NAME(new_proposal_id_not_flushed, "new proposal id not flushed")
    DEF_NAME(new_proposal_id_flushed, "new proposal id flushed")
    DEF_NAME(request_for_max_log_id, "request for max log id")
    DEF_NAME(max_log_ack_list_majority, "max log ack list majority")
    DEF_NAME(prepare_log_map, "prepare log map")
    DEF_NAME(confirmed_log_in_sw, "confirmed log in sw")
    DEF_NAME(fetch_max_lsn_to_reconfirming, "fetch max lsn to reconfirming")
    DEF_NAME(write_start_membership_finish, "write start membership finish")
    DEF_NAME(reconfirming_to_start_working, "reconfirming to start working")
    DEF_NAME(start_working_to_finish, "start working to finish")
    DEF_NAME(call_reconfirm_cnt, "call reconfirm cnt")
    DEF_NAME(new_start_id, "new start id")
    DEF_NAME(start_id, "start id")
    DEF_NAME(max_flushed_id, "max flushed id")
    DEF_NAME(new_proposal_id, "new proposal id")
    DEF_NAME(curr_proposal_id, "curr proposal id")
    DEF_NAME(curr_member_list, "curr member list")
    DEF_NAME(max_log_ack_list, "max log ack list")
    DEF_NAME(replay_start_working_log, "replay start working log")
    DEF_NAME(replay_rollback_to, "replay rollback to")
    DEF_NAME(replay_active_info, "replay active info")
    // perf event
    DEF_NAME(S_scan_iter, "storage: scan iterator")
    DEF_NAME(pc_fast_gen_stmt_key_end, "plan cache: fast genarate stmt_key")
    DEF_NAME(pc_plan_cache_hit, "plan cache: init")
    DEF_NAME(FROM_resolved, "resolver: from clause")
    DEF_NAME(SELECT_resolved, "resolver: select clause")
    DEF_NAME(START_WITH_resolved, "resolver: start with clause")
    DEF_NAME(CONNECT_BY_resolved, "resolver: connect by clause")
    DEF_NAME(WHERE_resolved, "resolver: where clause")
    DEF_NAME(GROUP_resolved, "resolver: group clause")
    DEF_NAME(HAVING_resolved, "resolver: having clause")
    DEF_NAME(NAMED_WINDOW_resolved, "resolver: named_window clause")
    DEF_NAME(ORDER_resolved, "resolver: order clause")
    DEF_NAME(TABLE_resolved, "resolver: from caluse")
    DEF_NAME(HINT_resolved, "resolver: hint")
    DEF_NAME(end_of_iter, "table scan: end of iteration")
    DEF_NAME(b_keyrange, "key range")
    DEF_NAME(get_session, "get session")
    DEF_NAME(release_schema_mgr, "release schema")
    DEF_NAME(do_process_begin, "process begin")
    DEF_NAME(plan_stat, "plan stat")
    DEF_NAME(clear_wb, "clear warning buffer")
    DEF_NAME(response_client_end, "response client")
    DEF_NAME(reset_op_ctx, "reset op_ctx")
    DEF_NAME(stmt_query_end, "stmt_query end")
    DEF_NAME(handle_text_query_end, "handle text_query")

    //dml event
    DEF_NAME(sync_auto_value, "sync_auto_value")

    DEF_NAME(value_start_next_row, "value_start_next_row")
    DEF_NAME(value_end_next_row, "value_end_next_row")
    DEF_NAME(value_start_calc_row, "value_start_calc_row")
    DEF_NAME(value_after_calc_row, "value_after_calc_row")

    DEF_NAME(delete_open, "delete_open")
    DEF_NAME(delete_start_delete, "delete_start_delete")
    DEF_NAME(delete_iter_end, "delete_iter_end")
    DEF_NAME(delete_start_next_row, "delete_start_next_row")
    DEF_NAME(delete_end_next_row, "delete_end_next_row")

    DEF_NAME(insert_open, "insert_open")
    DEF_NAME(insert_close, "inser_close")
    DEF_NAME(insert_start, "insert_start")
    DEF_NAME(insert_end, "insert_end")
    DEF_NAME(insert_start_next_row, "insert_start_next_row")
    DEF_NAME(insert_start_calc_row, "insert_start_calc_row")
    DEF_NAME(insert_end_next_row, "insert_end_next_row")
    DEF_NAME(insert_iter_end, "insert_iter_end")

    DEF_NAME(insertup_open, "insertup_open")
    DEF_NAME(insert_up_load_all_row, "insert_up_load_all_row")
    DEF_NAME(insert_up_try_insert, "insert_up_try_insert")
    DEF_NAME(insert_up_start_lookup, "insert_up_start_lookup")
    DEF_NAME(insert_up_start_shuff, "insert_up_start_shuff")
    DEF_NAME(insert_up_final_shuff, "insert_up_final_shuff")
    DEF_NAME(insert_up_inner_close, "insert_up_inner_close")

    DEF_NAME(insertup_start_do, "insertup_start_do")
    DEF_NAME(insertup_start_calc_insert_row, "insertup_start_calc_insert_row")
    DEF_NAME(insertup_start_insert_row, "insertup_start_insert_row")
    DEF_NAME(insertup_end_insert_row, "insertup_end_insert_row")
    DEF_NAME(insertup_end, "insertup_end")
    DEF_NAME(insertup_before_scan, "insertup_before_scan")
    DEF_NAME(insertup_start_update_row, "insertup_start_update_row")
    DEF_NAME(insertup_end_update_row, "insertup_end_update_row")
    DEF_NAME(insertup_start_calc_update_row, "insertup_start_calc_update_row")
    DEF_NAME(insertup_calc_new_row, "insertup_calc_new_row")
    DEF_NAME(insertup_auto_increment, "insertup_auto_increment")
    DEF_NAME(insertup_end_auto_increment, "insertup_end_auto_increment")

    DEF_NAME(replace_open, "replace_open")
    DEF_NAME(replace_load_all_row, "replace_load_all_row")
    DEF_NAME(replace_try_insert, "replace_try_insert")
    DEF_NAME(replace_start_lookup, "replace_start_lookup")
    DEF_NAME(replace_start_shuff, "replace_start_shuff")
    DEF_NAME(replace_final_shuff, "replace_final_shuff")
    DEF_NAME(replace_inner_close, "replace_inner_close")

    DEF_NAME(revert_insert_iter, "revert_insert_iter")
    DEF_NAME(revert_insert_iter_end, "revert_insert_iter_end")
    DEF_NAME(revert_scan_iter_end, "revert_scan_iter_end")
    DEF_NAME(replace_end, "replace_end")
    DEF_NAME(repalce_start_insert, "repalce_start_insert")
    DEF_NAME(replace_end_insert, "replace_end_insert")
    DEF_NAME(replace_start_delete, "replace_start_delete")
    DEF_NAME(replace_end_delete, "replace_end_delete")
    DEF_NAME(replace_start_insert, "replace_start_insert")
    DEF_NAME(replace_start_scan_row, "replace_start_scan_row")
    DEF_NAME(replace_start_table_scan, "replace_start_table_scan")
    DEF_NAME(replace_end_table_scan, "replace_end_table_scan")

    DEF_NAME(update_open, "update_open")
    DEF_NAME(update_iter_end, "update_iter_end")
    DEF_NAME(update_end, "update_end")
    DEF_NAME(update_start_next_row, "update_start_next_row")
    DEF_NAME(update_end_next_row, "update_end_next_row")
    DEF_NAME(update_start_check_row, "update_start_check_row")
    DEF_NAME(update_end_check_row, "update_end_check_row")

    DEF_NAME(before_calculate_row, "before_calculate_row")
    DEF_NAME(end_calculate_row, "end_calculate_row")
    DEF_NAME(row, "row")
    DEF_NAME(rowkey, "rowkey")
    DEF_NAME(store_rowkey, "store_rowkey")
    DEF_NAME(rowkey_len, "rowkey_len")
    DEF_NAME(table_id, "table_id")
    DEF_NAME(columns, "columns")
    DEF_NAME(trans_begin, "trans_begin")
    DEF_NAME(trans_end, "trans_end")
    DEF_NAME(trans_kill, "trans_kill")
    DEF_NAME(trans_pending, "trans_pending")
    DEF_NAME(trans_link, "trans_link")
    DEF_NAME(need_update_gts, "need_update_gts")
    DEF_NAME(update_gts, "update_gts")
    DEF_NAME(calc_checksum, "calc_checksum")
    DEF_NAME(commit, "commit")
    DEF_NAME(snapshot, "snapshot")
    DEF_NAME(snapshot_source, "snapshot source")
    DEF_NAME(snapshot_version, "snapshot version")
    DEF_NAME(snapshot_txid, "snapshot tx id")
    DEF_NAME(snapshot_scn, "snapshot scn")
    DEF_NAME(set, "set")
    DEF_NAME(lock, "lock")
    DEF_NAME(mget, "mget")
    DEF_NAME(replay, "replay")
    DEF_NAME(begin, "begin")
    DEF_NAME(end, "end")
    DEF_NAME(locked, "locked")
    DEF_NAME(abs_timeout, "abs_timeout")
    DEF_NAME(data_size, "data_size")
    DEF_NAME(dml, "dml")
    DEF_NAME(dml_type, "dml_type")
    DEF_NAME(modify_count, "modify_count")
    DEF_NAME(capacity, "capacity")
    DEF_NAME(read_row, "read_row")
    DEF_NAME(write_row, "write_row")
    DEF_NAME(memtable_ctx, "memtable_ctx")
    DEF_NAME(mtstat, "mtstat")
    DEF_NAME(atomic_pause, "atomic_pause")
    DEF_NAME(atomic_load, "atomic_laod")
    DEF_NAME(atomic_store, "atomic_store")
    DEF_NAME(atomic_faa, "atomic_faa")
    DEF_NAME(atomic_aaf, "atomic_aaf")
    DEF_NAME(atomic_fas, "atomic_fas")
    DEF_NAME(atomic_saf, "atomic_saf")
    DEF_NAME(atomic_tas, "atomic_tas")
    DEF_NAME(atomic_set, "atomic_set")
    DEF_NAME(atomic_vcas, "atomic_vcas")
    DEF_NAME(atomic_bcas, "atomic_bcas")
    DEF_NAME(atomic_Aaf, "atomic_Aaf")
    DEF_NAME(set_timeout, "set_timeout")
    DEF_NAME(start_time, "start_time")
    DEF_NAME(plan_timeout, "plan_timeout")
    DEF_NAME(hint_timeout, "hint_timeout")
    DEF_NAME(dfo_start, "dfo start")
    DEF_NAME(sqc_finish, "sqc finish")
    DEF_NAME(dfo_id, "dfo id")
    DEF_NAME(sqc_id, "sqc id")

    DEF_NAME(after_handle_log_task, "after_handle_log_task")
    DEF_NAME(after_submit_net_task, "after_submit_net_task")
    DEF_NAME(before_submit_flush_task, "before_submit_flush_task")
    DEF_NAME(after_submit_flush_task, "after_submit_flush_task")
    DEF_NAME(before_submit_log, "before_submit_log")
    DEF_NAME(after_submit_log, "after_submit_log")
    DEF_NAME(before_handle_log_task, "before_handle_log_task")
    DEF_NAME(rg_id, "rg_id")
    DEF_NAME(row_dml, "store_row_dml")
    DEF_NAME(first_dml, "first_dml")
    //rootserver
    DEF_NAME(generate_schema_finish, "generate schema finish")
    DEF_NAME_PAIR(failover_to_leader, "failover to leader")
    DEF_NAME_PAIR(wait_replay, "wait replay")
    DEF_NAME_PAIR(switch_partition, "switch partition to leader")
    DEF_NAME_PAIR(tenant_switch_partition, "tenant switch partition to leader")
    DEF_NAME_PAIR(flashback_partition, "flashback partition")
    DEF_NAME_PAIR(switchover_to_leader, "do switchover to leader")
    DEF_NAME_PAIR(get_epoch, "get switchover timestamp")
    DEF_NAME_PAIR(check_switchover_valid, "check can switchover to leader")
    DEF_NAME_PAIR(broadcast_cluster_stat, "broadcast_cluster_status")
    DEF_NAME_PAIR(check_log, "check log is latest")
    DEF_NAME_PAIR(check_member_enough, "check member enough")
    DEF_NAME_PAIR(switch_cluster, "switch cluster")
    DEF_NAME_PAIR(wait_flashback_info_dump, "wait flashback info dump")
    DEF_NAME_PAIR(write_cutdata, "write cut data clog")
    DEF_NAME_PAIR(tenant_write_cutdata, "tenant write cut data clog")
    DEF_NAME_PAIR(do_flashback, "do flashback")
    DEF_NAME_PAIR(wait_all_tenant_in_cutdata_status, "wait all tenant in cutdata status")
    DEF_NAME_PAIR(flashback_partitions, "iter all tenant do cutdata")
    DEF_NAME_PAIR(disconnect_cluster, "disconnect_cluster")
    DEF_NAME_PAIR(wait_obs_disconnect_cluster, "wait obs disconnect_cluster")
    // XA transaction
    DEF_NAME(xid, "XA transaction id")
    DEF_NAME(xa_start, "start XA transaction")
    DEF_NAME(xa_end, "end XA transaction")
    DEF_NAME(xa_prepare, "prepare XA transaction")
    DEF_NAME(rollback_to, "rollback_to")
    DEF_NAME(rollback_from, "rollback_from")
    DEF_NAME(gtrid, "global transaction id")
    DEF_NAME(bqual, "branch qualifier")
    DEF_NAME(is_stmt, "is normal dml stmt")
    DEF_NAME(xa_flag, "flag for xa transaciton")
    DEF_NAME(remote_pull, "remote pull operation")
    DEF_NAME(remote_push, "remote push operation")
    DEF_NAME(stmt_lock, "acquire stmt lock successfully")
    DEF_NAME(stmt_unlock, "release stmt lock successfully")
    DEF_NAME(terminate, "terminate xa transaction")
    DEF_NAME(xa_end_trans, "xa two phase commit/rollback")
    DEF_NAME(xa_one_phase, "xa one phase commit/rollback")
    DEF_NAME(xa_sync_response, "handle xa sync response")
    DEF_NAME(xa_start_request, "handle xa start request")
    DEF_NAME(xa_start_response, "handle xa start response")
    DEF_NAME(xa_start_stmt_request, "handle xa start stmt request")
    DEF_NAME(xa_start_stmt_response, "handle xa start stmt response")

    //////////////// << add new name BEFORE this line
    DEF_NAME(__PAIR_NAME_BEGIN__, "invalid")
    //////////////// << add pair events AFTER this line using DEF_NAME_PAIR
    // DEF_NAME_PAIR(x) defines both x_begin and x_end
    // pair events for trace
    DEF_NAME_PAIR(process, "process")
    DEF_NAME_PAIR(process_single_stmt, "process single statement")
    DEF_NAME_PAIR(process_execute_stmt, "process execute statement")
    DEF_NAME_PAIR(query, "query")
    DEF_NAME_PAIR(parse, "parse")
    DEF_NAME_PAIR(cache_get_plan, "pc get plan")
    DEF_NAME_PAIR(transform_with_outline, "transform_with_outline")
    DEF_NAME_PAIR(resolve, "resolve")
    DEF_NAME_PAIR(resolve_ins_tbl, "resolve insert table")
    DEF_NAME_PAIR(fill_column_conv, "fill column conv")
    DEF_NAME_PAIR(analyze_precalc_expr, "analyze precalc expr")
    DEF_NAME_PAIR(transform, "transform")
    DEF_NAME_PAIR(optimize, "optimizer")
    DEF_NAME_PAIR(bl_select, "bl_select")
    DEF_NAME_PAIR(bl_update, "bl_update")
    DEF_NAME_PAIR(bl_replace, "bl_replace")
    DEF_NAME_PAIR(bl_insert, "bl_insert")
    DEF_NAME_PAIR(bl_delete, "bl_delete")
    DEF_NAME_PAIR(cg, "cg")
    DEF_NAME_PAIR(exec, "execution")
    DEF_NAME_PAIR(job_exec_step, "job execute step")
    DEF_NAME_PAIR(distributed_schedule, "distributed schedule")
    DEF_NAME_PAIR(exec_plan, "execute plan")
    DEF_NAME_PAIR(parse_job, "parse job")
    DEF_NAME_PAIR(do_open_plan, "do open plan")
    DEF_NAME_PAIR(sql_start_stmt, "sql start stmt")
    DEF_NAME_PAIR(sql_start_participant, "sql start participant")
    DEF_NAME_PAIR(kv_get, "kv get")
    DEF_NAME_PAIR(plc_get_from_cache, "plc get from cache")
    DEF_NAME_PAIR(plc_serialize, "plc serialize")

    DEF_NAME_PAIR(exec_dist_plan, "exec dist plan")
    DEF_NAME_PAIR(exec_remote_plan, "exec remote plan")
    DEF_NAME_PAIR(exec_mini_plan, "exec mini plan")

    DEF_NAME_PAIR(end_participant, "start_end_participant")
    DEF_NAME_PAIR(close_plan, "start_close_plan")
    DEF_NAME_PAIR(auto_end_plan, "start_auto_end_plan")
    DEF_NAME_PAIR(storage_table_scan, "table scan")
    DEF_NAME_PAIR(get_location_cache, "get location cache")
    DEF_NAME_PAIR(calc_partition_location, "calc partition location")
    DEF_NAME_PAIR(sync_end_trans_callback, "sync end trans callback")
    DEF_NAME_PAIR(async_end_trans_callback, "async end trans callback")
    DEF_NAME_PAIR(calc_tablet_location, "calc tablet location")
    // pair events for obperf
    // shorthand:
    // T: transaction
    // S: storage
    // pc: plan cache
    // E: executor
    DEF_NAME_PAIR(S_table_scan, "storage: table scan")
    DEF_NAME_PAIR(S_revert_iter, "storage: revert iterator")
    DEF_NAME_PAIR(S_delete_rows, "storage: delete rows")
    DEF_NAME_PAIR(S_delete_row, "storage: delete row")
    DEF_NAME_PAIR(S_insert_rows, "storage: insert rows")
    DEF_NAME_PAIR(S_insert_rows2, "storage: insert rows2")
    DEF_NAME_PAIR(S_update_rows, "storage: update rows")
    DEF_NAME_PAIR(S_update_row, "storage: update row")
    DEF_NAME_PAIR(S_lock_rows, "storage: lock rows")
    DEF_NAME_PAIR(S_lock_rows2, "storage: lock rows2")
    DEF_NAME_PAIR(S_revert_insert_iter, "storage: revert insert iterator")
    DEF_NAME_PAIR(T_end_trans_callback, "transaction: end_trans callback")
    DEF_NAME_PAIR(T_start_trans, "transaction: start transaction")
    DEF_NAME_PAIR(T_end_trans, "transaction: end transaction")
    DEF_NAME_PAIR(T_start_stmt, "transaction: start statement")
    DEF_NAME_PAIR(T_end_stmt, "transaction: end statement")
    DEF_NAME_PAIR(T_start_part, "transaction: start participant")
    DEF_NAME_PAIR(T_end_part, "transaction: end participant")
    DEF_NAME_PAIR(T_part_destroy, "transaction: part_trans_ctx destroy")
    DEF_NAME_PAIR(T_destroy_callback, "transaction: destroy callback")
    DEF_NAME_PAIR(E_analysis_plan, "executor: analysis plan")
    DEF_NAME_PAIR(E_create_job, "executor: create job")
    DEF_NAME_PAIR(E_execute_plan, "executor: execute plan")
    DEF_NAME_PAIR(E_local_schedule, "executor: local schedule")
    DEF_NAME_PAIR(E_local_exec, "executor: local execute")
    DEF_NAME_PAIR(E_get_exec_task, "executor: get task")
    DEF_NAME_PAIR(rs_open, "open result set")
    DEF_NAME_PAIR(rs_close, "close result set")
    DEF_NAME_PAIR(exec_result_close, "close exec result")
    DEF_NAME_PAIR(pc_get_plan, "plan cache: get plan")
    DEF_NAME_PAIR(pc_init_result_set, "plan cache: init result set")
    DEF_NAME_PAIR(prepare_scan, "table scan: prepare")
    DEF_NAME_PAIR(get_schema_mgr, "get schema")
    DEF_NAME_PAIR(end_trans_sync_wait, "sql: sync wait end_trans")
    DEF_NAME_PAIR(exec_context_destructor, "sql: exec_context destruct")
    DEF_NAME_PAIR(stat_start, "statistics: start record")
    DEF_NAME_PAIR(stat_finish, "statistics: finish record")
    DEF_NAME_PAIR(record_stat, "statistics: record stat")
    DEF_NAME_PAIR(audit, "statistics: audit")
    // rootserver section
    DEF_NAME_PAIR(create_table, "create table")
    DEF_NAME_PAIR(create_user_tables, "create user tables")
    DEF_NAME_PAIR(create_tables_in_trans, "create_tables_in_trans")
    DEF_NAME_PAIR(public_schema, "public_schema")
    DEF_NAME_PAIR(alloc_replica, "alloc_replica")
    DEF_NAME_PAIR(alloc_partition, "alloc_partition")
    DEF_NAME_PAIR(create_table_partitions, "create_table_partitions")
    DEF_NAME_PAIR(batch_create_partition, "batch_create_partition")
    DEF_NAME_PAIR(obs_create_partition, "observer batch_create_partition")
    DEF_NAME_PAIR(wait_election, "wait leader election")
    DEF_NAME_PAIR(update_flag_replica, "update flag_replica")
    DEF_NAME_PAIR(fill_flag_replica, "fill flag replica")
    DEF_NAME_PAIR(operator_create_table, "operator_create_table")
    DEF_NAME_PAIR(init_batch_args, "init_batch_args")
    DEF_NAME_PAIR(for_each_dest, "for_each_dest")
    DEF_NAME(create_tablegroup_partitions_begin, "create_tablegroup_partitions_begin")
    DEF_NAME(create_tablegroup_begin, "create_tablegroup_begin")
    DEF_NAME(write_tablegroup_schema_begin, "write_tablegroup_schema_begin")
    DEF_NAME(publish_tablegroup_schema_begin, "publish_tablegroup_schema_begin")
    DEF_NAME(publish_tablegroup_schema_end, "publish_tablegroup_schema_end")
    DEF_NAME(wait_tablegroup_leader_end, "wait_tablegroup_leader_end")
    DEF_NAME(generate_schema_start, "generate_schema_start")
    DEF_NAME(generate_schema_index, "generate_schema_index")
    DEF_NAME(generate_schema_lob, "generate_schema_lob")
    DEF_NAME(generate_schema_vertial_partition, "generate_schema_vertial_partition")
    DEF_NAME(status, "status")
    DEF_NAME(get_trans_result, "get_trans_result")
    DEF_NAME(deserialize, "deserialize")

    // location cache related
    DEF_NAME_PAIR(renew_loc_by_sql, "renew loc by sql")
    DEF_NAME_PAIR(fetch_vtable_loc, "fetch vtable loc")
    DEF_NAME_PAIR(renew_loc_by_rpc, "renew loc by rpc")
    DEF_NAME_PAIR(storage_estimation, "storage_row_estimation")

    //heartbeat
    DEF_NAME_PAIR(renew_lease, "renew_lease")
    DEF_NAME_PAIR(renew_master_rs, "renew_master_rs")
    DEF_NAME_PAIR(do_renew_lease, "do_renew_lease")
    DEF_NAME_PAIR(send_heartbeat, "send_heartbeat")
    DEF_NAME(update_cluster_info, "update_cluster_info")
    DEF_NAME(update_master_key_info, "update_master_key_info")
    DEF_NAME(do_heartbeat_event, "do_heartbeat_event")
    // << add pair events BEFORE this line
    DEF_NAME(NAME_COUNT, "invalid")
#endif
////////////////////////////////////////////////////////////////

#ifndef _OB_NAME_ID_DEF_H
#define _OB_NAME_ID_DEF_H 1
#include <stdint.h>
namespace oceanbase
{
namespace name
{
enum ObNameId
{
#define DEF_NAME(name_sym, description) name_sym,
#define DEF_NAME_PAIR(name_sym, description) \
  DEF_NAME(name_sym ## _begin, description " begin")    \
  DEF_NAME(name_sym ## _end, description " end")
#include "ob_name_id_def.h"
#undef DEF_NAME
#undef DEF_NAME_PAIR
};

// get name at runtime
const char* get_name(int32_t id);
const char* get_description(int32_t id);
} // end namespace name_id_map
} // end namespace oceanbase

#define OB_ID(name_sym) (::oceanbase::name::name_sym)
#define NAME(name_id) (::oceanbase::name::get_name(name_id))
#define Y(x) OB_ID(x), x
#define Y_(x) OB_ID(x), x ##_

#endif /* _OB_NAME_ID_DEF_H */

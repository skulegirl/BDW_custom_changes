<?php

/**
 * Implement hook_rules_action_info().
 */
function BDW_custom_changes_rules_action_info() {
  return array( 
    'BDW_custom_changes_rules_action_mark_pm_read' => array( 
      'label' => t('Mark all private messages in thread as read'),
      'group' => t('Private messages'),
      'parameter' => array(
        'tid' => array(
          'type' => 'integer',
          'label' => t('Thread ID of the private message to mark read'),
          'description' => t('Enter the MID of the message to mark as read.'),
        ),
        'recipient' => array(
          'type' => 'user',
          'label' => t('Recipient to mark thread read for'),
          'desription' => t('Enter the user that the thread should be marked as read for'),
        ),
      ),
    ), 
  );
}

function BDW_custom_changes_rules_action_mark_pm_read($tid, $recipient) {
  $params = array(
    ':recipient' => $recipient->uid,
    ':thread' => $tid,
  ); 

  // Record which messages will change status
  $result = db_query("SELECT mid FROM {pm_index} WHERE is_new = 1 AND recipient = :recipient and type IN ('user', 'hidden') AND thread_id = :thread", $params);
  $changed = $result->fetchCol();

  // Update the status of the threads.
  db_update('pm_index')
    ->fields(array('is_new' => 0))
    ->condition('thread_id', $tid)
    ->condition('recipient', $recipient->uid)
    ->condition('type', array('user', 'hidden'))
    ->execute();

  // Allow modules to respond to the status changes.
  foreach ($changed as $mid) {
    module_invoke_all('privatemsg_message_status_changed', $mid, 0, $recipient);
  }
}


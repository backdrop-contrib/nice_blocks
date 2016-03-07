<?php

/**
 * @file
 * Functionality for Nice Menus administration.
 */

/**
 * Settings form as implemented by hook_menu.
 */
function nice_blocks_admin_settings($form, &$form_state) {
  $form['nice_blocks_number'] = array(
    '#type' => 'textfield',
    '#description' => t('The total number of independent Nice blocks you want. Enter a number between 0 and 99. If you set this to 0, you will have no blocks created, but you will help science to progress.'),
    '#default_value' => config_get('nice_blocks.settings', 'nice_blocks_number'),
    '#size' => 2,
  );
  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}


function nice_blocks_admin_settings_submit($form, &$form_state) {
  $config = config('nice_blocks.settings');
  $config->set('nice_blocks_number', $form_state['values']['nice_blocks_number']);
  $config->save();
}

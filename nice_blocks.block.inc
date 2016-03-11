<?php

/**
 *  NiceBlock extends Block
 *  
 *  This class allows us to display a particular node in a block.
 */
class NiceBlock extends Block {
  /**
   * A copy of the node title.
   */
  var $clone_title;

  /**
   *  Sets title text on draggable block panel in Layout builder.
   */
  function getAdminTitle() {
    return t('Content block');
  }

  /**
   * Return a preview for this block.
   */
  function getAdminPreview() {
    return t('Displays content of node ID: @nid', array('@nid' => $this->settings['nid']));
  }

  /**
   *  Sets block subject on block view.
   */
  function getTitle() {
    $title = NULL;
    $node = node_load($this->settings['nid']);
    if ($this->settings['title_display'] === LAYOUT_TITLE_CUSTOM && $this->settings['title']) {
      $title = check_plain($this->settings['title']);
    }
    elseif ($this->settings['title_display'] === LAYOUT_TITLE_DEFAULT) {
      $title = $node->title;
      if (empty($this->settings['leave_node_title'])) {
        $title = $this->clone_title;
      }
    }

    if ($title && $this->settings['link_node_title']) {
      $title = l($title, 'node/' . $node->nid);
    }

    return $title;
  }

  /**
   *  Sets block content on block view.
   */
  function getContent() {
    $node = node_load($this->settings['nid']);
    // Prevent display if no access to the node.
    if (!node_access('view', $node)) {
      return;
    }
    // Hide node title if not required.
    if (empty($this->settings['leave_node_title'])) {
      // But first save the title so it can be shown if needed as the block
      // title.
      $this->clone_title = $node->title;
        // Do not clear title in entity cache
//      $node = clone($node); //++
//      $node->title = NULL; // see $variables['page'] in nice_blocks_preprocess_node()
    }

    $content = node_view($node, $this->settings['build_mode']);

//   see nice_blocks_preprocess_node()
    // Hide links if they've been suppressed.
//  if (empty($this->settings['links'])) {
//    $content['links']['#access'] = FALSE;
//  }

    // Add theme hook suggestion.
//- $content['#theme'] = 'node__content-block-' . $this->settings['nid'];
    $content['#theme'] = 'node__content_block_' . $this->settings['nid']; //+
    $content['#theme'] = 'node__content_block'; //++
    $content['#content_block_id'] = !!$this->settings['nid'];
  //$content['#content_block_links'] = !!$this->settings['links'];
    $content['#content_block_more'] = !!$this->settings['more'];
    $content['#content_block_comments'] = !!$this->settings['comments'];
    $content['#content_block_submitted'] = !!$this->settings['submitted'];
    $content['#content_block_title'] = !!$this->settings['leave_node_title'];

    return $content;
  }

  /**
   *  Builds the block's settingsiguration form.
   */
  function form(&$form, &$form_state) {
    parent::form($form, $form_state);
    $form['title_display']['title_display']['#options'][LAYOUT_TITLE_DEFAULT] = t('Use node title');
    $form['title_display']['title_display']['#weight'] = -50;
    $settings = $this->settings;

    $form['nid'] = array(
      '#title' => t('Enter the title or NID of a node'),
      '#type' => 'textfield',
      '#maxlength' => 512,
      '#default_value' => !empty($settings['nid']) ? $settings['nid'] : NULL,
      '#autocomplete_path' => 'nice_blocks/autocomplete',
    );

    $form['leave_node_title'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['leave_node_title']),
      '#title' => t('Leave node title'),
      '#description' => t('Advanced: if checked, display the node title; this can cause the node title to appear twice unless you select "none" for the node title type.'),
    );

    $form['link_node_title'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['link_node_title']),
      '#title' => t('Link the node title to the node'),
      '#description' => t('Check this box if you would like the block title to link to the node.'),
    );

    $form['comments'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['comments']),
      '#title' => t('Include node links for "add comment".'),
    );

    $form['more'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['more']),
      '#title' => t('Include node links for "read more".'),
    );

    $form['submitted'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['submitted']),
      '#title' => t('Include node "submitted".'),
    );

    $entity = entity_get_info('node');
    $build_mode_options = array();
    foreach ($entity['view modes'] as $mode => $option) {
      $build_mode_options[$mode] = $option['label'];
    }

    $form['build_mode'] = array(
      '#title' => t('Build mode'),
      '#type' => 'select',
      '#description' => t('Select a build mode for this node.'),
      '#options' => $build_mode_options,
      '#default_value' => isset($settings['build_mode']) ? $settings['build_mode'] : 'teaser',
    );
  }

  /**
   * Validates the form settings.
   */
  function formValidate($form, &$form_state) {
    parent::formValidate($form, $form_state);

    module_load_include('inc', 'nice_blocks', 'nice_blocks.pages');
    if ($nid = nice_blocks_autocomplete_validate($form_state['values']['nid'])) {
      $form_state['values']['nid'] = $nid;
    }
    else {
      form_error($form['nid'], t('Sorry, no node matching <em>%title</em> was found.', array('%title' => $form_state['values']['nid'])));
    }
  }

  /**
   * Submit handler to save the form settings.
   */
  function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);
    foreach (array('nid', 'submitted', 'more', 'comments', 'leave_node_title', 'link_node_title', 'build_mode') as $key) {
      $this->settings[$key] = $form_state['values'][$key];
    }
  }
}

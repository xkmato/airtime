<?php

use Airtime\CcWebstream;
use Airtime\CcWebstreamQuery;

class WebstreamController extends Zend_Controller_Action
{
    public function init()
    {
        $ajaxContext = $this->_helper->getHelper('AjaxContext');
        $ajaxContext->addActionContext('new', 'json')
                    ->addActionContext('save', 'json')
                    ->addActionContext('edit', 'json')
                    ->addActionContext('delete', 'json')
                    ->initContext();
    }

    public function newAction()
    {
    	$service = new Application_Service_WebstreamService();
    	$form = $service->makeWebstreamForm(null);
    	
    	$form->setDefaults(array(
    		'name' => 'Unititled Webstream',
    		'hours' => 0,
    		'mins' => 30,
    	));
    	
    	$this->view->html = $form->render();
    }

    public function editAction()
    {
        $request = $this->getRequest();
        $id = $request->getParam("id");
        
        $service = new Application_Service_WebstreamService();
    	$form = $service->makeWebstreamForm($id, true);

        $this->view->html = $form->render();
    }

    public function deleteAction()
    {
        $request = $this->getRequest();
        $ids = $request->getParam("ids");

        $service = new Application_Service_WebstreamService();
        $service->deleteWebstreams($ids);
    }

    public function saveAction()
    {
        $request = $this->getRequest();
        $parameters = array();
        
        foreach (array('id','hours', 'mins', 'name','description','url') as $p) {
            $parameters[$p] = trim($request->getParam($p));
        }
        
        Logging::info($parameters);
        
        $service = new Application_Service_WebstreamService();
        $form = $service->makeWebstreamForm(null);
        
        if ($form->isValid($parameters)) {
        	Logging::info("form is valid");
        	
        	$values = $form->getValues();
        	Logging::info($values);
        	
        	$ws = $service->saveWebstream($values);
        	
        	$this->view->statusMessage = "<div class='success'>"._("Webstream saved.")."</div>";
        }
        else {
        	Logging::info("form is not valid");
        	
        	$this->view->statusMessage = "<div class='errors'>"._("Invalid form values.")."</div>";
        	$this->view->errors = $form->getMessages();
        }
    }
}

package main.java.fr.soleil.comete.bptApp.view;


import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Scanner;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.WindowConstants;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;

import fr.soleil.comete.box.scalarbox.StringScalarBox;
import fr.soleil.comete.swing.Label;
import fr.soleil.comete.swing.StringButton;
import fr.soleil.comete.tango.data.service.TangoKey;
import fr.soleil.comete.tango.data.service.TangoKeyTool;

public class ConfigPanel implements Runnable  {
	private static final String BPT_DEVICE_CLASS = "BeamPositionTracking";
	private static final String X_PID_PROP = "XPIDDefinition";
	private static final String Y_PID_PROP = "YPIDDefinition";
	private static final String X_MIN_POSITION = "XAxisMinPosition";
	private static final String X_MAX_POSITION = "XAxisMaxPosition";
	private static final String Y_MIN_POSITION = "YAxisMinPosition";
	private static final String Y_MAX_POSITION = "YAxisMaxPosition";
	private static final String X_CALIBRATION_STEP_NUMBER = "CalibrationStepNbXAxis";
	private static final String Y_CALIBRATION_STEP_NUMBER = "CalibrationStepNbYAxis";
	private static final String AS_DEVICE_ADRESS = "ActuatorSystemDeviceAdress";
	private static final String WARM_BEAM_LINE_TRANSLATION_THRESHOLD = "bpt_ui_warmBeamLineTranslationThreshold";
    private static final String AXES_ALIASES = "AxesAliases";
	boolean _bptDeviceReadyToConnect;
	boolean _bptDeviceAddressHasChange;
	String _bptDeviceAdress;
	JFrame _mainFrame;
	ConfigPanel _configPanel;
	static ControlPanel _controlPanel;
	StringButton _controlRedirection;
	

    Label targetDeviceClassLabel = new Label();
	
	//Right panel
	JPanel _rightPanel;
	Label _asAdressLabel;
	JTextField _bptDeviceAdressTextField;
	StringButton _connectBPT;
	Label _bptState;
	Label _asState;
	Label _bptConnectionStatus;
	
	JPanel _asDescPanel;
	//Left panel
	JPanel _leftPanel;
	// BPT Info
	Label bptMode;

	// X Axis Info
	Label xPid;
	Label xMinPosition;
	Label xMaxPosition;
	Label xCalibrationStepNumber;
	// Y Axis Info
	Label yPid;
	Label yMinPosition;
	Label yMaxPosition;
	Label yCalibrationStepNumber;
	//Axes alias
	Label _axesAliases;
	String _xAlias;
	String _yAlias;
	
	//PID info
	boolean _pidInfoInitDone;
	//AS device init
	boolean _asDeviceInitDone;
	Label isXPidInUse;
	Label xPCoef;
	Label xICoef;
	Label xDCoef;
	Label isYPidInUse;
	Label yPCoef;
	Label yICoef;
	Label yDCoef;
	
	//specific ui properties
	Label _bpt_ui_warmBeamLineTranslationThreshold;
	Label _bpt_ui_coldBeamLineTranslationThreshold;
	Label _bpt_ui_lastBeamLineInUse;
	Label _bpt_ui_favoriteBPTDevice;
	
	//ConfigPannel State
	boolean _bptReady;
	boolean _asReady;
	boolean _axesAliasesInit;
	/****************************************************************
	 *  ConfigPanel() 
	 *  
	 *
	 * **************************************************************/
	public ConfigPanel(){
		_bptDeviceReadyToConnect = false;
		_bptDeviceAddressHasChange = false;
		_axesAliasesInit = false;
		_asDeviceInitDone = false;
		_pidInfoInitDone = false;
		_bptReady = false;
		_asReady = false;
		_configPanel = this;
		_xAlias = "x";
		_yAlias = "y";
		initComponents();
		initGui();
		run();
	}
	/****************************************************************
	 *  refreshInterface() 
	 *  
	 *
	 * **************************************************************/
	public void refreshInterface(){
		//BPT connection is ok
		if (_bptReady && _asReady /*&& _axesAliasesInit*/){
			_controlRedirection.setVisible(true);
			_rightPanel.setVisible(true);
		}
		//BPT connection not ok
		else {
			if(_bptDeviceReadyToConnect)
				if(checkBptDeviceClass()){
					initializeBPTConnection();
					_bptDeviceReadyToConnect = false;
				}
			_controlRedirection.setVisible(false);
			_rightPanel.setVisible(false);
		}
		//If bpt is ready, initialize as
		if(!_asReady && _bptReady){
			_asDescPanel.setVisible(false);
			initASDevice();
		}
		//If as device is ready, print panel
		if(_asReady)
			_asDescPanel.setVisible(true);
		else
			_asDescPanel.setVisible(false);
		
		if (!_pidInfoInitDone && _bptReady)
			_pidInfoInitDone = parsePIDs();
		if(!_axesAliasesInit && _bptReady)
			initAliases();
	}
	/****************************************************************
	 *  initASDevice() 
	 *  
	 *
	 * **************************************************************/
	public void initASDevice(){
		if (!_asAdressLabel.getText().isEmpty())
			connectAs();		
	}
	/****************************************************************
	 *  initComponents() 
	 *  
	 *  To init components
	 * **************************************************************/
	private void initComponents(){
		//Init control "redirection"
		_controlRedirection = new StringButton();
		_controlRedirection.setText("BPT Control");
		_controlRedirection.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				//Object control panel not created yet
				if (_controlPanel == null && !_asAdressLabel.getText().isEmpty()){	
					

					//TODO GET new LABELS SOMEWHERE !!!!					
					_controlPanel = new ControlPanel(_bptDeviceAdressTextField.getText(),
							 						  _asAdressLabel.getText(),
							 						  _configPanel, 
							 						 _xAlias, _yAlias);
					 //start control panel thread in new thread
					 new Thread(new Runnable() {
					      public void run() {
					    	  _controlPanel.run();
					    	  }				    		  
					      }
				         ).start();
				}
				//If control panel started, then show it
				else {
					if(_bptDeviceAddressHasChange){
						_controlPanel.updateDevices(_bptDeviceAdressTextField.getText(), _asAdressLabel.getText());
						_bptDeviceAddressHasChange = false;
					}
					_controlPanel.showControlPanel();
					hideConfigPanel();
				}
			}
		});
		
		//Devices Address
		_bptDeviceAdressTextField = new JTextField("Simulation/BeamPositionTracking/beampositiontracking", 20);
		//States
		_bptState = new Label();
		_asState = new Label();		

		//axes alias 
		_axesAliases = new Label();		

		//Connect BPT button
		_connectBPT = new StringButton();
		_connectBPT.setText("BPT connection");
		_connectBPT.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				connectBpt();
			}
		});
		
		//connection status
		_bptConnectionStatus = new Label();
		//X properties info
		xPid = new Label();
		xMinPosition = new Label();
		xMaxPosition = new Label();
		xCalibrationStepNumber = new Label();
		//Y properties info
		yPid = new Label();
		yMinPosition = new Label();
		yMaxPosition = new Label();
		yCalibrationStepNumber = new Label();	
		//PIDs
		isXPidInUse = new Label();
		xPCoef = new Label();
		xICoef = new Label();
		xDCoef = new Label();
		isYPidInUse = new Label();
		yPCoef = new Label();
		yICoef = new Label();
		yDCoef = new Label();
		//AS 
		_asDescPanel = new JPanel(new GridLayout(2,0));
		_asAdressLabel = new Label();
		//UI - specifics
		_bpt_ui_warmBeamLineTranslationThreshold = new Label();
		_bpt_ui_coldBeamLineTranslationThreshold = new Label();
		_bpt_ui_lastBeamLineInUse = new Label();
		_bpt_ui_favoriteBPTDevice = new Label();
		
	}
	
	/****************************************************************
	 *  connectBpt() 
	 *  
	 *  
	 * **************************************************************/
	public void connectBpt(){
		
		//check if bpt address has changed => to update control panel 
		if (!_bptDeviceAdressTextField.getText().equals(_bptDeviceAdress))
			_bptDeviceAddressHasChange = true;
	
		_bptReady = false;
		_asReady = false;
		_bptConnectionStatus.setText("No BeamPositionTracking device connected yet !");
		_bptDeviceAdress = _bptDeviceAdressTextField.getText();
		StringScalarBox boxStr = new StringScalarBox();  
        TangoKey bptStatekey = new TangoKey();
        try{
        	TangoKeyTool.registerDeviceClass(bptStatekey, _bptDeviceAdress);
        	boxStr.connectWidget(targetDeviceClassLabel, bptStatekey);
        	//boxStr.co
        	 _bptDeviceReadyToConnect = true;
        }catch (Exception E){

        	System.out.println("exception connection");
        	_bptConnectionStatus.setText("Counldn't connect to distant device"
        								+ "\nPlease check device adress !");	
        	_bptDeviceReadyToConnect = false;	
        }
	}
	/****************************************************************
	 *  initializeBPTConnection() 
	 *  
	 *  
	 * **************************************************************/
	public void initializeBPTConnection(){
		connectProperty(xPid, _bptDeviceAdress,X_PID_PROP );
		connectProperty(yPid, _bptDeviceAdress,Y_PID_PROP );
		connectProperty(xCalibrationStepNumber, _bptDeviceAdress,X_CALIBRATION_STEP_NUMBER );
		connectProperty(yCalibrationStepNumber, _bptDeviceAdress,Y_CALIBRATION_STEP_NUMBER );
		connectProperty(_asAdressLabel,_bptDeviceAdress, AS_DEVICE_ADRESS );
		connectProperty(_axesAliases, _bptDeviceAdress, AXES_ALIASES);
		connectSpecificsProperties(_bpt_ui_warmBeamLineTranslationThreshold, _bptDeviceAdress, WARM_BEAM_LINE_TRANSLATION_THRESHOLD);
		
		_bptConnectionStatus.setText("Device BeamTrackingPosition : " + _bptDeviceAdress + " is connected !");
		this._bptReady = true;	
	}
	/****************************************************************
	 *  checkBptDeviceClass() 
	 *  
	 *  
	 * **************************************************************/
	public boolean checkBptDeviceClass(){
		String distantDeviceClass = targetDeviceClassLabel.getText();
    	
    	if(!distantDeviceClass.isEmpty()){
	        if (distantDeviceClass.equals(BPT_DEVICE_CLASS)){
	        	return true;
	        }
	        else{
	        	_bptConnectionStatus.setText("Device adress incorrect"
											+" , this is not a BeamPositionTracking device!");
	        	_bptDeviceReadyToConnect = false;
	        	return false;
	        }
    	}
    	return false;
	}
	/****************************************************************
	 *  initAliases() 
	 *  
	 *  To init axes aliases
	 * **************************************************************/
	public void initAliases(){
		if (!_axesAliases.getText().equals("")){
			_axesAliasesInit = true;
			String [] aliases = _axesAliases.getText().split("\n");

			_xAlias = aliases[0];
			_yAlias = aliases[1];
		}
	}
	/****************************************************************
	 *  ParsePIDs(Label xPid, Label yPid) 
	 *  
	 *  
	 * **************************************************************/
	public boolean parsePIDs(){
		boolean xPidInfoInitDone = false;
		boolean yPidInfoInitDone = false;
		String xPIDstr = xPid.getText();
		String yPIDstr = yPid.getText();

		//Don't know exactly when property will be filled up
		if (!xPIDstr.isEmpty()){
			Scanner s = new Scanner(xPIDstr).useDelimiter("::");
			isXPidInUse.setText(s.next());
			xPCoef.setText(s.next());
			xICoef.setText(s.next());
			xDCoef.setText(s.next());
			xPidInfoInitDone = true;
		}
		//Don't know exactly when property will be filled up
		if (!yPIDstr.isEmpty()){
			Scanner s = new Scanner(yPIDstr).useDelimiter("::");
			isYPidInUse.setText(s.next());
			yPCoef.setText(s.next());
			yICoef.setText(s.next());
			yDCoef.setText(s.next());
			yPidInfoInitDone = true;
		}
		
		return (xPidInfoInitDone && yPidInfoInitDone);
	}
	/****************************************************************
	 *  connectAs() 
	 *  
	 *  
	 * **************************************************************/
	public void connectAs(){
		String asDeviceAdress = _asAdressLabel.getText();
		if(initTangoconnection(asDeviceAdress, _asState)){
			//INIT PROPERTIES VALUES  !
			connectProperty(xMinPosition, asDeviceAdress,X_MIN_POSITION );
			connectProperty(xMaxPosition, asDeviceAdress,X_MAX_POSITION );
			connectProperty(yMinPosition, asDeviceAdress,Y_MIN_POSITION );
			connectProperty(yMaxPosition, asDeviceAdress,Y_MAX_POSITION );
			this._asReady = true;
		}
		else 
			this._asReady = false;
	}
	/****************************************************************
	 *  initTangoconnection(String deviceAdress, Label stateListener) 
	 *  
	 *  
	 * **************************************************************/
	public boolean initTangoconnection(String deviceAdress, Label stateListener){
		StringScalarBox boxStr = new StringScalarBox();  
		try{
			//Connect device state
	        TangoKey stateKey = new TangoKey();
	        TangoKeyTool.registerAttribute(stateKey, deviceAdress+ "/state");
	        boxStr.connectWidget(stateListener, stateKey);
		}catch(Exception E){
			//couldn't connect device 
			return false;
		}
		return true;
	}
	/****************************************************************
	 *  initGui() 
	 *  
	 *  To user interface
	 * **************************************************************/
	private void initGui(){
		
        _mainFrame = new JFrame(ConfigPanel.class.getSimpleName());
        _mainFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        _mainFrame.setLayout(new GridLayout(0,2));

        //Init panels
        _leftPanel = initLeftPanel();
        _rightPanel = initRightPanel();
        _mainFrame.add(_leftPanel);        
        _mainFrame.add(_rightPanel);
        
        Dimension appDim = new Dimension(1500, 600);
        _mainFrame.setSize(appDim);
        _mainFrame.setLocationRelativeTo(null);
        _mainFrame.setVisible(true);
	}
	/****************************************************************
	 *  initRightPanel() 
	 *  
	 *  To init right panel
	 * **************************************************************/
	public JPanel initRightPanel(){
		JPanel rightPanel = new JPanel(new GridLayout(4,0));
		Border rightPanelBord = BorderFactory.createTitledBorder("BPT Configuration");
			
		//AXES LIMITS PROPERTIES		
		JPanel axesLimitsPanel = new JPanel(new GridLayout(0,2));
		axesLimitsPanel.setBorder(BorderFactory.createTitledBorder("AXES LIMITS" ));
		//X limits
		JPanel xLimitsPanel = new JPanel(new GridLayout(2,2));	
		xLimitsPanel.setBorder(BorderFactory.createTitledBorder("X Axis" ));
		Label xMinLimLab = new Label();
		xMinLimLab.setText("	Min limit ");
		Label xMaxLimLab = new Label();
		xMaxLimLab.setText("	Max limit ");
		xLimitsPanel.add(xMinLimLab);
		xLimitsPanel.add(xMinPosition);
		xLimitsPanel.add(xMaxLimLab);
		xLimitsPanel.add(xMaxPosition);
		//Y limits
		JPanel yLimitsPanel = new JPanel(new GridLayout(2,2));	
		yLimitsPanel.setBorder(BorderFactory.createTitledBorder("Y Axis" ));
		Label yMinLimLab = new Label();
		yMinLimLab.setText("	Min limit ");
		Label yMaxLimLab = new Label();
		yMaxLimLab.setText("	Max limit ");
		yLimitsPanel.add(yMinLimLab);
		yLimitsPanel.add(yMinPosition);
		yLimitsPanel.add(yMaxLimLab);
		yLimitsPanel.add(yMaxPosition);
		axesLimitsPanel.add(xLimitsPanel);
		axesLimitsPanel.add(yLimitsPanel);
		
		//PID VALUES
		JPanel axesPIDsPanel = new JPanel(new GridLayout(0,2));
		axesPIDsPanel.setBorder(BorderFactory.createTitledBorder("AXES PIDs" ));

		Label pXCoefLab = new Label();
		pXCoefLab.setText("P coef :");
		Label iXCoefLab = new Label();
		iXCoefLab.setText("I coef :");
		Label dXCoefLab = new Label();
		dXCoefLab.setText("D coef :");
		JPanel xPIDPanel = new JPanel(new GridLayout(0,3));	
		xPIDPanel.setBorder(BorderFactory.createTitledBorder("X PID "));
		
		JPanel xPPanel = new JPanel(new GridLayout(0,2));
		xPPanel.add(pXCoefLab);
		xPPanel.add(xPCoef);
		JPanel xIPanel = new JPanel(new GridLayout(0,2));
		xIPanel.add(iXCoefLab);
		xIPanel.add(xICoef);
		JPanel xDPanel = new JPanel(new GridLayout(0,2));
		xDPanel.add(dXCoefLab);
		xDPanel.add(xDCoef);
		
		xPIDPanel.add(xPPanel);
		xPIDPanel.add(xIPanel);
		xPIDPanel.add(xDPanel);
		//X limits
		Label pYCoefLab = new Label();
		pYCoefLab.setText("P coef :");
		Label iYCoefLab = new Label();
		iYCoefLab.setText("I coef :");
		Label dYCoefLab = new Label();
		dYCoefLab.setText("D coef :");
		JPanel yPIDPanel = new JPanel(new GridLayout(0,3));	
		yPIDPanel.setBorder(BorderFactory.createTitledBorder("Y PID "));
		
		JPanel yPPanel = new JPanel(new GridLayout(0,2));
		yPPanel.add(pYCoefLab);
		yPPanel.add(yPCoef);
		JPanel yIPanel = new JPanel(new GridLayout(0,2));
		yIPanel.add(iYCoefLab);
		yIPanel.add(yICoef);
		JPanel yDPanel = new JPanel(new GridLayout(0,2));
		yDPanel.add(dYCoefLab);
		yDPanel.add(yDCoef);
		
		yPIDPanel.add(yPPanel);
		yPIDPanel.add(yIPanel);
		yPIDPanel.add(yDPanel);
		
		axesPIDsPanel.add(xPIDPanel);
		axesPIDsPanel.add(yPIDPanel);

		rightPanel.add(axesLimitsPanel);
		rightPanel.add(axesPIDsPanel);
		rightPanel.setBorder(rightPanelBord);
		return rightPanel;
	}
	/****************************************************************
	 *  initRightPanel() 
	 *  
	 *  To init right panel
	 * **************************************************************/
	public JPanel initLeftPanel(){
		JPanel leftPanel = new JPanel(new GridLayout(3,0));
		Border leftPanelBord = BorderFactory.createTitledBorder("Devices connections");
		
		//BPT connection
		_bptDeviceAdressTextField.setBorder(new TitledBorder("BPT adress"));
		JPanel deviceBptconnectionStateButton = new JPanel(new GridLayout(0,2));
		deviceBptconnectionStateButton.add(_connectBPT);
		deviceBptconnectionStateButton.add(_bptState);
		JPanel deviceBPTconnection = new JPanel(new GridLayout(3,0));
		deviceBPTconnection.add(_bptDeviceAdressTextField);
		deviceBPTconnection.add(deviceBptconnectionStateButton);
		_bptConnectionStatus.setBorder(new TitledBorder("BeamTrackingPosition device connection state"));

		_bptConnectionStatus.setText("No BeamPositionTracking device connected yet !");
		deviceBPTconnection.add(_bptConnectionStatus);
		
		//AS connection
		/*asDeviceAdressTextField.setBorder(new TitledBorder("AS adress"));
		JPanel deviceASconnectionStateButton = new JPanel(new GridLayout(0,2));
		deviceASconnectionStateButton.add(_connectAS);
		deviceASconnectionStateButton.add(_asState);
		JPanel deviceASconnection = new JPanel(new GridLayout(2,0));
		deviceASconnection.add(asDeviceAdressTextField);
		deviceASconnection.add(deviceASconnectionStateButton);*/
		

		
		_asDescPanel.setBorder(new TitledBorder("Actuator system paired :"));
		_asDescPanel.add(_asAdressLabel);
		_asDescPanel.add(_asState);
		
		leftPanel.add(deviceBPTconnection);
		leftPanel.add(_asDescPanel);
		leftPanel.add(_controlRedirection);
		leftPanel.setBorder(leftPanelBord);
		return leftPanel;
	}
	/****************************************************************
	 *  hideConfigPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void hideConfigPanel(){
		_mainFrame.setVisible(false);
	}
	/****************************************************************
	 *  showConfigPanel() 
	 *  
	 *  
	 * **************************************************************/
	public void showConfigPanel(){
		_mainFrame.setVisible(true);
	}
	/****************************************************************
	 *  connectStrLabel(Label targetListener, String targetStr, String deviceAdress) 
	 *  
	 *  
	 * **************************************************************/
	public void connectProperty(Label targetListener, String deviceAdress, String devicePropertyName){
		StringScalarBox boxStr = new StringScalarBox();  
		 
        TangoKey bptStatekey = new TangoKey();
        TangoKeyTool.registerDeviceProperty(bptStatekey, deviceAdress, devicePropertyName);
        boxStr.connectWidget(targetListener, bptStatekey);
	}
	
	/****************************************************************
	 *  connectStrLabel(Label targetListener, String targetStr, String deviceAdress) 
	 *  
	 *  
	 * **************************************************************/
	public void connectSpecificsProperties(Label targetListener, String deviceAdress, String propertyName){
		StringScalarBox boxStr = new StringScalarBox();  
		 
        TangoKey bptStatekey = new TangoKey();
        
        //Specifics => database???
        //TangoFactory 
       //TangoKeyTool.(bptStatekey, deviceAdress, targetStr);
        //boxStr.connectWidget(targetListener, bptStatekey);
		
	}
	/****************************************************************
	 *  run() 
	 *  
	 *  
	 * **************************************************************/
	public void run() {
		while (!Thread.currentThread().isInterrupted()) {
			try {
				Thread.sleep(100);
				// To interface
				refreshInterface();
			} catch (InterruptedException e) {
			}
		}
		
	}
}

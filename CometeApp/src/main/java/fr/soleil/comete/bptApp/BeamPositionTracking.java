package fr.soleil.comete.bptApp;

import fr.soleil.comete.bptApp.view.ConfigPanel;
import fr.soleil.comete.bptApp.view.ControlPanel;
import fr.soleil.comete.swing.Panel;

public class BeamPositionTracking {

	final static Panel panel = new Panel();
	static ControlPanel controlPanel;
	static ConfigPanel configPanel;
	public static void main(String[] args) {
		configPanel = new ConfigPanel();
	}
}

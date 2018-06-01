package main.java.fr.soleil.comete.bptApp;

import fr.soleil.comete.swing.Panel;
import main.java.fr.soleil.comete.bptApp.view.ConfigPanel;
import main.java.fr.soleil.comete.bptApp.view.ControlPanel;

public class BeamPositionTracking {

	final static Panel panel = new Panel();
	static ControlPanel controlPanel;
	static ConfigPanel configPanel;
	public static void main(String[] args) {
		configPanel = new ConfigPanel();
	}
}

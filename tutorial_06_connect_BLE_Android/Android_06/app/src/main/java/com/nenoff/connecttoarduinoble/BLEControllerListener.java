/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 *
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 *
 */

package com.nenoff.connecttoarduinoble;

public interface BLEControllerListener {
    public void BLEControllerConnected();
    public void BLEControllerDisconnected();
    public void BLEDeviceFound(String name, String address);
}

<?php

if (getenv("AS_PHPUNIT")) {
    require_once 'PHPUnit.php';
    abstract class ASTestFramework extends PHPUnit_Framework_TestCase {

    }
}
 /* otherwise, the dependency is provided by the loader */

require_once 'Util.inc';

abstract class AerospikeTestCommon extends ASTestFramework
{
    static $_handle = NULL;
    static $_oo = NULL;
    private $old_eh = NULL;

    function skipALL() {

    }

    protected function setUp(&$db, &$key) {
        $this->setErrorsRaiseException(true);
        $this->handle = make_handle();
        $this->oo = make_handle_oo();
        $this->skipALL();
    }

    protected function tearDown(&$db, &$key) {

    }

    /**
     * Returns a resource
     */
    protected function getPersistHandle() {
        if (!self::$_handle) {
            self::$_handle = make_handle();
        }
        return self::$_handle;
    }

    /**
     * return a handle
     */
    protected function getPersistOO() {
        if (!self::$_oo) {
            self::$_oo = make_handle_oo();
        }
        return self::$_oo;
    }

    /**
     * Make all warnings raise an error exception. This is the
     * default, but might help to do so explicitly.
     *
     * @param $b boolean, whether to raise an ErrorException for the
     * error handler
     *
     * @return the old error handler
     */
    protected function setErrorsRaiseException($b = true) {
        $ret = NULL;
        if ($b) {
            $ret = set_error_handler(
                function ($errno,$errstr,$errfile,$errline,$errctx) {
                    throw new ErrorException(
                        $errstr, $errno, $errno, $errfile, $errline);
                });
        } else {

            $ret = set_error_handler(function() { });
        }

        $this->old_eh = $ret;
        return $ret;
    }

    /**
     * Should be called when wishing to restore default behavior
     */
    protected function restoreErrorHandler() {
        set_error_handler($this->old_eh);
    }

    /**
     * Capture errors, warnings etc. and append them to the variable
     * @param str a string which is appended to
     * @return the old error handler
     */
    protected function setCaptureError(&$str) {
        $ret = set_error_handler(
            function($errno, $errstr, $errfile, $errline, $errctx)
            use (&$str) {
                $str .= $errstr;
            }
        );

        $this->old_eh = $ret;
        return $ret;
    }

    /**
     * Invoke a function in a 'safe' context and capture any fatals
     *
     * @param func the function to call
     * @param args the arguments to pass to the function
     * @param expr a regular expression the output should match
     */
    protected function
    assertFatalRegexp($func, $args, $expr, $maxwait = 10, $msg = "") {

        $tmpfile = tempnam("", "php_aerospike_tests-");
        $pid = pcntl_fork();

        if (!$pid) {
            fclose(STDOUT);
            global $STDOUT;
            global $STDERR;

            $STDERR = fopen($tmpfile, "a");
            $STDOUT = STDERR;

            pcntl_alarm($maxwait);
            call_user_func_array($func, $args);

            exit(0);
        }

        pcntl_waitpid($pid, $status);
        $lines = file($tmpfile);
		die("line count: ".count($lines));
        $txt = join(" ", $lines);

        $found_match = preg_match($expr, $txt) > 0;
        $this->assertRegExp($expr, $txt, $msg);
        return true;
    }

    /**
     * Invoke a function in a 'safe' context and assert that the exception's
     * message matches the regular expression
     *
     * @param func the function to invoke
     * @param args the arguments to pass
     * @param expr a regular expression the output should match
     */
    protected function
    assertExceptionRegexp($func, $args, $expr, $msg = "") {
        $errmsg = NULL;
        try {
            call_user_func_array($func, $args);
        } catch (Exception $exc) {
            $errmsg = $exc->getMessage();
        }

        $this->assertNotNull($errmsg);
        $this->assertRegExp($expr, $errmsg, $msg);
        return true;
    }
}

?>
